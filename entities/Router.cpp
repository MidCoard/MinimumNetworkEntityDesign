#include "Router.h"

#include <utility>
#include "DHCPHelper.h"
#include "DHCPOfferPacket.h"
#include "DHCPACKPacket.h"
#include "DHCPNAKPacket.h"
#include "DHCPDeclinePacket.h"
#include "ICMPReplyPacket.h"
#include "ICMPPacket.h"

Router::Router(Network *network, int node, std::map<int, RouterConfiguration *> routerConfigurations) : NetworkEntity(
		network, node, new RouterNetworkLayer(this)), routerConfigurations(std::move(routerConfigurations)) {
}

std::vector<std::string> Router::createLayers(int node, std::vector<int> ids) {
	for (int id: ids) {
		if (!this->routerConfigurations.empty()) {
			RouterConfiguration *routerConfiguration = routerConfigurations.at(id);
			((RouterNetworkLayer *) this->layer)->setIPConfiguration(id, routerConfiguration->getSegment(),
			                                                         routerConfiguration->getMask(),
			                                                         routerConfiguration->getGateway());
			auto *linkLayer = new LinkLayer(id, this);
			if (routerConfiguration->getMAC() != nullptr)
				linkLayer->setMAC(id, *routerConfiguration->getMAC());
			else
				linkLayer->setMAC(id, generateMAC());
            std::cout<<"Router "<<this->node<<" "<<id<<" MAC ADDRESS "<<linkLayer->getMAC().str()<<std::endl;
			this->layer->addLowerLayer(linkLayer);
			auto *physicalLayer = new PhysicalLayer(id, this, routerConfiguration->getLinkAddress() == nullptr
			                                                  ? generateLinkAddress(node, id)
			                                                  : *routerConfiguration->getLinkAddress(),
			                                        routerConfiguration->getPhysicalAddress() == nullptr
			                                        ? generatePhysicalAddress(node, id)
			                                        : *routerConfiguration->getPhysicalAddress());
			linkLayer->addLowerLayer(physicalLayer);
		} else {
			((RouterNetworkLayer *) this->layer)->setIPConfiguration(id, nullptr, nullptr, nullptr);
			auto *linkLayer = new LinkLayer(id, this);
			linkLayer->setMAC(id, generateMAC());
			this->layer->addLowerLayer(linkLayer);
			auto *physicalLayer = new PhysicalLayer(id, this, generateLinkAddress(node, id),
			                                        generatePhysicalAddress(node, id));
			linkLayer->addLowerLayer(physicalLayer);
		}
	}
	return this->layer->generateGraph(node);
}

bool Router::isIPAvailable() {
	return true;
}

Router::~Router() {
	for (auto &routerConfiguration: routerConfigurations) {
		delete routerConfiguration.second;
	}
	auto *networkLayer = (RouterNetworkLayer *) this->layer;
	for (int i = 0; i < this->layer->lowerLayers.size(); i++) {
		IPConfiguration ipConfiguration = networkLayer->getIPConfiguration(i);
		delete ipConfiguration.segment;
		delete ipConfiguration.mask;
		delete ipConfiguration.gateway;
	}
}

void Router::start() {
	NetworkEntity::start();
	auto *networkLayer = (NetworkLayer *) this->layer;
	networkLayer->sendDHCP();
	dhcp::request(networkLayer);
}

bool Router::isRouter() {
	return true;
}

Router::Router(Network *network, int node,std::map<int, RouterConfiguration *> routerConfigurations,  NetworkLayer* layer) : NetworkEntity(network, node, layer),routerConfigurations(std::move(routerConfigurations)) {}

RouterConfiguration::RouterConfiguration(IP *segment, IP *mask, IP *gateway, MAC *mac, INetAddress *linkAddress,
                                         INetAddress *physicalAddress)
		: segment(segment), mask(mask), gateway(gateway), mac(mac), linkAddress(linkAddress),
		  physicalAddress(physicalAddress) {}

IP *RouterConfiguration::getSegment() {
	return this->segment;
}

IP *RouterConfiguration::getMask() {
	return this->mask;
}

IP *RouterConfiguration::getGateway() {
	return this->gateway;
}

MAC *RouterConfiguration::getMAC() {
	return this->mac;
}

INetAddress *RouterConfiguration::getPhysicalAddress() {
	return this->physicalAddress;
}

INetAddress *RouterConfiguration::getLinkAddress() {
	return this->linkAddress;
}

RouterConfiguration::~RouterConfiguration() {
	delete mac;
	delete linkAddress;
	delete physicalAddress;
}

RouterNetworkLayer::RouterNetworkLayer(NetworkEntity *networkEntity) : NetworkLayer(networkEntity) {}

RouterNetworkLayer::RouterNetworkLayer(int id, NetworkEntity *networkEntity) : NetworkLayer(id, networkEntity) {}

void RouterNetworkLayer::handleReceive(int id, Block *block) {
	this->checkDHCP();
	if (block->getSendCount() < 0)
		return;
	if (block->getRemaining() < 8)
		return;
	IP source = block->readIP();
	IP destination = block->readIP();
	IPConfiguration ipConfiguration = configurations.at(id);
	if (destination.isBroadcast()) {
		// broadcast
		// first check if ip valid
		if (!this->isIPValid)
			return;
		// ip valid the dhcpTable is also valid
		// then check if it is dhcp request or dhcp discover
		unsigned char header;
		block->read(&header, 1);
		switch (header) {
			case 0x01:
				if (id != 0 && source == LOCAL0) {
					// receive dhcp discover, as this is a dhcp server, so handle it!
					MAC mac = block->readMAC();
					unsigned char useSegment;
					block->read(&useSegment, 1);
					log("Receive DHCP discover from " + mac.str() + " useSegment: " + std::to_string(useSegment));
					if (useSegment) {
						std::pair<std::pair<IP, IP>, long long int> apply = this->tables[id]->applySegment();
						if (apply.second != -1) {
							auto *packet = new DHCPOfferPacket(mac, apply.first.first, apply.first.second,
							                                   *ipConfiguration.getGateway(), apply.second);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						}
					} else {
						std::pair<IP, long long int> app = this->tables[id]->apply();
						if (app.second != -1) {
							auto *packet = new DHCPOfferPacket(mac, app.first, *ipConfiguration.getMask(),
							                                   *ipConfiguration.getGateway(), app.second);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						}
					}
				}
				break;
			case 0x03:
				if (id != 0 && source == LOCAL0) {
					// receive dhcp request, as this is a dhcp server, so handle it!
					IP segment = block->readIP();
					IP mask = block->readIP();
					MAC mac = block->readMAC();
					int dhcpID = block->readInt();
					unsigned char useSegment;
					block->read(&useSegment, 1);
					log("DHCP request from with segment " + segment.str() + " and mask " + mask.str() + " and mac " +
					    mac.str() + " and dhcpID " + std::to_string(dhcpID));
					if (useSegment) {
						if (this->tables[id]->applyItOrMAC(&segment, &mask, mac, dhcpID)) {
							log("Send DHCPACK(Segment) to " + segment.str() + " from " +
							    ipConfiguration.getSegment()->str() + " with segment " + segment.str() + " and mask " +
							    mask.str() + " and mac " + mac.str() + " and dhcpID " + std::to_string(dhcpID));
							this->routeTable.updateLong(segment, mask, 0, segment, id);
							auto *packet = new DHCPACKPacket(mac, *ipConfiguration.getSegment(), segment, mask,
							                                 *ipConfiguration.getGateway(),
							                                 kDHCPTime);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						} else {
							error("Send DHCPNAK(Segment) to " + segment.str() + " from " +
							      ipConfiguration.getSegment()->str() + " with segment " + segment.str() +
							      " and mask " + mask.str() + " and mac " + mac.str() + " and dhcpID " +
							      std::to_string(dhcpID));
							this->tables[id]->print();
							auto *packet = new DHCPNAKPacket(mac, *ipConfiguration.getSegment());
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						}
					} else {
						if (this->tables[id]->applyItOrMAC(segment, mac, dhcpID)) {
							log("Send DHCPACK to " + segment.str() + " from " + ipConfiguration.getSegment()->str() +
							    " with segment " + segment.str() + " and mask " + mask.str() + " and mac " + mac.str() +
							    " and dhcpID " + std::to_string(dhcpID));
							auto *packet = new DHCPACKPacket(mac, *ipConfiguration.getSegment(), segment, mask,
							                                 *ipConfiguration.getGateway(),
							                                 kDHCPTime);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						} else {
							error("Send DHCPNAK to " + segment.str() + " from " + ipConfiguration.getSegment()->str() +
							      " with segment " + segment.str() + " and mask " + mask.str() + " and mac " +
							      mac.str() + " and dhcpID " + std::to_string(dhcpID));
							this->tables[id]->print();
							auto *packet = new DHCPNAKPacket(mac, *ipConfiguration.getSegment());
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						}
					}
				}
				break;
			case 0x06:
				if (id != 0 && source == LOCAL0) {
					// dhcp decline
					IP segment = block->readIP();
					IP mask = block->readIP();
					MAC mac = block->readMAC();
					unsigned char useSegment;
					block->read(&useSegment, 1);
					log("Receiving DHCP decline from " + segment.str() + " with segment " + segment.str() +
					    " and mask " + mask.str() + " and mac " + mac.str());
					if (useSegment)
						this->tables[id]->decline(segment, mask, mac);
					else
						this->tables[id]->decline(segment, mac);
				}
				break;
			default: {
				// this is broadcast packet
				auto *newBlock = new Block();
				newBlock->writeMAC(BROADCAST_MAC);
				newBlock->write(0);
				newBlock->writeIP(source);
				newBlock->writeIP(destination);
				newBlock->write(header);
				newBlock->writeBlock(block);
				newBlock->flip();
				for (auto &layer: this->lowerLayers)
					if (layer->getID() != id)
						// transfer to all lower layers except the one that sent the packet
						layer->send(newBlock->copy());
				delete newBlock;
			}

		}

	} else if (!this->isIPValid) {
		// this packet is for me
		// why for me ?
		// because this is dhcp packet, the mac is to me!
		unsigned char header;
		block->read(&header, 1);
		switch (header) {
			case 0x02:
				if (id == 0) {// receive dhcp offer
					if (ipConfiguration.isValid())
						return;
					IP segment = block->readIP();
					IP mask = block->readIP();
					IP gateway = block->readIP();
					auto *linkLayer = (LinkLayer *) this->lowerLayers[id];
                    MAC mac = this->arpTable.lookup(segment);
                    int count = 0;
					this->offerable = false;
                    while(mac.isBroadcast() && count < 5) {
                        linkLayer->sendARP(segment, segment);
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                        count++;
                    }
					this->offerable = true;
					if (!mac.isBroadcast()) {
						// one have already got the ip (maybe static ip)
						auto *packet = new DHCPDeclinePacket(segment, mask, mac, true);
						auto *newBlock = packet->createBlock();
						delete packet;
						this->lowerLayers[id]->send(newBlock);
					} else {
						this->dhcpID = block->readInt();
						delete ipConfiguration.getSegment();
						delete ipConfiguration.getMask();
						delete ipConfiguration.getGateway();
						this->setIPConfiguration(0, new IP(segment), new IP(mask), new IP(gateway));
						this->sendDHCP();
					}
				}
				break;
			case 0x04:
				if (id == 0) {
					// receive dhcp ack
					IP segment = block->readIP();
					IP mask = block->readIP();
					IP gateway = block->readIP();
					this->log("receive DHCP_ACK get segment: " + segment.str() + " mask: " + mask.str() +
					          " gateway: " + gateway.str());
					delete ipConfiguration.getSegment();
					delete ipConfiguration.getMask();
					delete ipConfiguration.getGateway();
					this->setIPConfiguration(0, new IP(segment), new IP(mask), new IP(gateway));
					this->startDHCP = util::getNowTime();
					this->duration = block->readLong();
					this->isIPValid = true;
					for (auto &table: this->tables)
						delete table;
					this->tables.clear();
					this->tables.push_back(new DHCPTable(segment, mask));
					auto *layer = (LinkLayer *) this->lowerLayers[0];
					bool flag = this->tables[0]->directApply(segment, layer->getMAC());
					if (!flag)
						throw std::runtime_error("apply its segment failed");
					this->routeTable.updateLong(LOCAL0, LOCAL0, 10, gateway, 0);
					for (int i = 1; i < this->lowerLayers.size(); i++) {
						auto *layer = (LinkLayer *) this->lowerLayers[i];
						IPConfiguration configuration = this->getIPConfiguration(i);
						if (configuration.getSegment()->isBroadcast() || configuration.getMask()->isBroadcast()) {
							std::pair<IP, IP> pair = this->tables[0]->directApplySegment(layer->getMAC());
							if (pair.first.isBroadcast())
								throw std::runtime_error("no ip segment available");
							delete configuration.getSegment();
							delete configuration.getMask();
							delete configuration.getGateway();
							this->tables.push_back(new DHCPTable(pair.first, pair.second));
							IP gateway = this->tables[i]->directApply(layer->getMAC());
							if (gateway.isBroadcast())
								throw std::runtime_error("no gateway available");
							this->setIPConfiguration(i, new IP(pair.first), new IP(pair.second), new IP(gateway));
						} else {
							IP segment = *configuration.getSegment();
							IP mask = *configuration.getMask();
							delete configuration.getSegment();
							delete configuration.getMask();
							bool flag = this->tables[0]->applyIt(&segment, &mask, layer->getMAC(), -1);
							if (!flag) {
								std::pair<IP, IP> pair = this->tables[0]->directApplySegment(layer->getMAC());
								if (pair.first.isBroadcast())
									throw std::runtime_error("no ip segment available");
								delete configuration.getGateway();
								this->tables.push_back(new DHCPTable(pair.first, pair.second));
								IP gateway = this->tables[i]->directApply(layer->getMAC());
								if (gateway.isBroadcast())
									throw std::runtime_error("no gateway available");
								this->setIPConfiguration(i, new IP(pair.first), new IP(pair.second), new IP(gateway));
							} else {
								if (configuration.getGateway() == nullptr) {
									this->tables.push_back(
											new DHCPTable(segment, mask));
									IP gateway = this->tables[i]->directApply(layer->getMAC());
									if (gateway.isBroadcast())
										throw std::runtime_error("no gateway available");
									this->setIPConfiguration(i, new IP(segment), new IP(mask),
									                         new IP(gateway));
								} else {
									this->tables.push_back(
											new DHCPTable(segment, mask));
									flag = this->tables[i]->directApply(*configuration.getGateway(), layer->getMAC());
									if (!flag) {
										IP gateway = this->tables[i]->directApply(layer->getMAC());
										if (gateway.isBroadcast())
											throw std::runtime_error("no gateway available");
										delete configuration.getGateway();
										this->setIPConfiguration(i, new IP(segment), new IP(mask),
										                         new IP(gateway));
									} else {
										this->setIPConfiguration(i, new IP(segment), new IP(mask),
										                         configuration.getGateway());
									}
								}
							}
						}
						IPConfiguration ipConfig = this->getIPConfiguration(i);
						this->routeTable.updateLong(*ipConfig.getSegment(), *ipConfig.getMask(), 0,
						                            *ipConfig.getSegment(), i);
					}
				}
				break;
			case 0x05:
				if (id == 0) {
					// receive dhcp nak
					this->dhcpID = -1;
					delete ipConfiguration.getSegment();
					delete ipConfiguration.getMask();
					delete ipConfiguration.getGateway();
					this->setIPConfiguration(0, nullptr, nullptr, nullptr);
				}
				break;
			default: {
				error("DefaultRouter is not available");
			}
		}
	} else if (destination == *ipConfiguration.getSegment()) {
		unsigned char header;
		block->read(&header, 1);
		switch (header) {
			case 0x03:
				if (id != 0) {
					// receive uni-cast dhcp request this is for renewal
					IP segment = block->readIP();
					IP mask = block->readIP();
					MAC mac = block->readMAC();
					block->readInt();
					unsigned char useSegment;
					block->read(&useSegment, 1);
					if (useSegment) {
						if (this->tables[id]->renewal(segment, mask, mac)) {
							auto *packet = new DHCPACKPacket(mac, *ipConfiguration.getGateway(), segment, mask,
							                                 *ipConfiguration.getGateway(),
							                                 kDHCPTime);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						} else {
							auto *packet = new DHCPNAKPacket(mac, *ipConfiguration.getSegment());
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						}
					} else {
						if (this->tables[id]->renewal(segment, mac)) {
							auto *packet = new DHCPACKPacket(mac, *ipConfiguration.getGateway(), segment, mask,
							                                 *ipConfiguration.getGateway(),
							                                 kDHCPTime);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						} else {
							auto *packet = new DHCPNAKPacket(mac, *ipConfiguration.getSegment());
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						}
					}
				}
				break;
			case 0x04: {
				if (id == 0) {
					IP segment = block->readIP();
					IP mask = block->readIP();
					IP gateway = block->readIP();
					this->log("receive DHCP_ACK get segment: " + segment.str() + " mask: " + mask.str() +
					          " gateway: " + gateway.str());
					delete ipConfiguration.getSegment();
					delete ipConfiguration.getMask();
					delete ipConfiguration.getGateway();
					this->setIPConfiguration(0, new IP(segment), new IP(mask), new IP(gateway));
					this->startDHCP = util::getNowTime();
					this->duration = block->readLong();
					this->isIPValid = true;
				}
				break;
			}
			case 0x07:
				if (id != 0) {
					IP segment = block->readIP();
					IP mask = block->readIP();
					MAC mac = block->readMAC();
					unsigned char useSegment;
					block->read(&useSegment, 1);
					log("receive DHCP_RELEASE get segment: " + segment.str() + " mask: " + mask.str() + " mac: " +
					    mac.str());
					this->tables[id]->release(segment, mask, mac, useSegment);
				}
				break;
			case 0x20: {
				IP segment = block->readIP();
				IP query = block->readIP();
				// destination is my address
				if (query == destination) {
					auto *packet = new ICMPReplyPacket(segment, query, source,
					                                   ICMPReplyStatus::kICMPReplyStatusSuccess);
					auto *newBlock = packet->createBlock();
					delete packet;
					this->send(newBlock);
				} else {
					kExecutor.submit([this, segment, query, source]() {
						if (this->icmpTable.lookup(segment, query) == 2) {
							this->icmpTable.remove(segment, query);
							auto *packet = new ICMPReplyPacket(segment, query, source,
							                                   ICMPReplyStatus::kICMPReplyStatusUnreachable);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->send(newBlock);
						}
					}, std::chrono::milliseconds(2000));
					this->icmpTable.add(segment, query);
					auto *packet = new ICMPPacket(segment, query, query);
					auto *newBlock = packet->createBlock();
					delete packet;
					this->send(newBlock);
				}
				break;
			}
			case 0x21: {
				IP segment = block->readIP();
				IP query = block->readIP();
				unsigned char flag;
				block->read(&flag, 1);
				if (icmpTable.lookupAndUpdate(segment, query, flag) != -1) {
					if (segment == destination) {
						// this is my request
						this->handleICMP(query, flag ? ICMPReplyStatus::kICMPReplyStatusUnreachable
						                             : ICMPReplyStatus::kICMPReplyStatusSuccess);
					} else {
						// i am the transfer
						auto *packet = new ICMPReplyPacket(segment, query, segment,
						                                   flag ? ICMPReplyStatus::kICMPReplyStatusUnreachable
						                                        : ICMPReplyStatus::kICMPReplyStatusSuccess);
						auto *newBlock = packet->createBlock();
						delete packet;
						this->send(newBlock);
					}
				}
				break;
			}
			default: {
				error("Unknown protocol type " + std::to_string(header) + " from " + source.str() + " to " +
				      destination.str());
			}
		}

	} else {
		// transfer
		std::pair<IP, int> nextHop = routeTable.lookup(destination);
		if (nextHop.second == -1)
			// do not know how to transfer
			return;

		// find the next hop
		IPConfiguration configuration = this->configurations.at(nextHop.second);
		if (nextHop.first == *configuration.getSegment() && nextHop.first == destination) {
			auto *newBlock = new Block();
			newBlock->writeIP(source);
			newBlock->writeIP(destination);
			newBlock->writeBlock(block);
			newBlock->flip();
			this->receive(nextHop.second, newBlock);
		} else {
			IP ip = nextHop.first == *configuration.getSegment() ? destination : nextHop.first;
			MAC mac = this->arpTable.lookup(ip);
			if (mac.isBroadcast()) {
				((LinkLayer *) this->lowerLayers[nextHop.second])->sendARP(*ipConfiguration.getSegment(), ip);
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				auto *newBlock = new Block(block->getSendCount() - 1);
				newBlock->writeIP(source);
				newBlock->writeIP(destination);
				newBlock->writeBlock(block);
				newBlock->flip();
				this->receive(id, newBlock);
			} else {
				auto *newBlock = new Block();
				newBlock->writeMAC(mac);
				newBlock->write(0);
				newBlock->writeIP(source);
				newBlock->writeIP(destination);
				newBlock->writeBlock(block);
				newBlock->flip();
				this->lowerLayers[nextHop.second]->send(newBlock);
			}
		}
	}
}

void RouterNetworkLayer::sendDHCP() {
	this->sendDHCP0(true);
}

RouterNetworkLayer::~RouterNetworkLayer() {
	for (auto &table: this->tables)
		delete table;
}

IP RouterNetworkLayer::getIP(int id) {
	if (!isIPValid)
		throw std::invalid_argument("IP is not valid");
	return *configurations.at(id).getSegment();
}

void RouterNetworkLayer::sendDHCPRelease() {
	this->sendDHCPRelease0(true);
}

void RouterNetworkLayer::sendDHCPRenewal() {
    this->sendDHCPRenewal0(true);
}
