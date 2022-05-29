#include "Router.h"
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

bool Router::isRouter() {
	return true;
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
	((NetworkLayer *) this->layer)->sendDHCP();
	dhcp::request((NetworkLayer *) this->layer);
}

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
	delete segment;
	delete mask;
	delete gateway;
	delete mac;
	delete linkAddress;
	delete physicalAddress;
}

RouterNetworkLayer::RouterNetworkLayer(NetworkEntity *networkEntity) : NetworkLayer(networkEntity) {}

RouterNetworkLayer::RouterNetworkLayer(int id, NetworkEntity *networkEntity) : NetworkLayer(id, networkEntity) {}

void RouterNetworkLayer::handleReceive(int id, Block *block) {
	routeTable.check();
	if (this->isIPValid)
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
					if (useSegment) {
						std::pair<IP, IP> apply = this->table->applySegment();
						if (!apply.first.isBroadcast()) {
							int dhcpID = this->table->dhcpID++;
							auto *packet = new DHCPOfferPacket(mac, apply.first, apply.second,
							                                   *ipConfiguration.getGateway(), dhcpID);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						}
					} else {
						IP apply = this->table->apply();
						if (!apply.isBroadcast()) {
							int dhcpID = this->table->dhcpID++;
							auto *packet = new DHCPOfferPacket(mac, apply, *ipConfiguration.getMask(),
							                                   *ipConfiguration.getGateway(), dhcpID);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						}
					}
					break;
				}
			case 0x03:
				if (id != 0 && source == LOCAL0) {
					// receive dhcp request, as this is a dhcp server, so handle it!
					IP segment = block->readIP();
					IP mask = block->readIP();
					MAC mac = block->readMAC();
					int dhcpID = block->readInt();
					unsigned char useSegment;
					block->read(&useSegment, 1);
					if (useSegment) {
						if (this->table->tryApply(segment, mask, mac, dhcpID)) {
							auto *packet = new DHCPACKPacket(mac, *ipConfiguration.getSegment(), segment, mask,
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
						if (this->table->tryApply(segment, mac, dhcpID)) {
							auto *packet = new DHCPACKPacket(mac, *ipConfiguration.getSegment(), segment, mask,
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
					break;
				}
			case 0x06:
				if (id != 0 && source == LOCAL0) {
					// dhcp decline
					IP segment = block->readIP();
					IP mask = block->readIP();
					MAC mac = block->readMAC();
					unsigned char useSegment;
					block->read(&useSegment, 1);
					if (useSegment)
						this->table->tryApply(segment, mask, mac, -1);
					else
						this->table->tryApply(segment, mac, -1);
					break;
				}
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
					linkLayer->sendARP(segment, segment);
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					MAC mac = this->arpTable.lookup(segment);
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
					break;
				}
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
					this->startDHCP = std::chrono::system_clock::now().time_since_epoch().count();
					this->duration = block->readLong();
					this->isIPValid = true;
					delete this->table;
					this->table = new DHCPTable(segment, mask);
					auto *linkLayer = (LinkLayer *) this->lowerLayers[0];
					this->table->tryApply(segment, linkLayer->getMAC(), -1);
					for (int i = 1; i < this->lowerLayers.size(); i++) {
						auto *linkLayer = (LinkLayer *) this->lowerLayers[i];
						//todo
						this->table->tryApply(segment, linkLayer->getMAC(), i);
					}
					break;
				}
			case 0x05:
				if (id == 0) {
					// receive dhcp nak
					this->dhcpID = -1;
					delete ipConfiguration.getSegment();
					delete ipConfiguration.getMask();
					delete ipConfiguration.getGateway();
					this->setIPConfiguration(0, nullptr, nullptr, nullptr);
					break;
				}
			default: {
				error("Router is not available");
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
						if (this->table->renewal(segment, mask, mac)) {
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
						if (this->table->renewal(segment, mac)) {
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
					break;
				}
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
						this->icmpTable.remove(segment, query);
						auto *packet = new ICMPReplyPacket(segment, query, source,
						                                   ICMPReplyStatus::kICMPReplyStatusUnreachable);
						auto *newBlock = packet->createBlock();
						delete packet;
						this->send(newBlock);
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
				if (query == destination) {
					this->icmpTable.update(segment, query, flag);
				} else {
					// i am the transfer
					auto *packet = new ICMPReplyPacket(segment, query, segment,
					                                   flag ? ICMPReplyStatus::kICMPReplyStatusUnreachable
					                                        : ICMPReplyStatus::kICMPReplyStatusSuccess);
					auto *newBlock = packet->createBlock();
					delete packet;
					this->send(newBlock);
				}
				break;
			}
			default: {
				error("Unknown protocol type");
			}
		}

	} else {
		// transfer
		std::pair<IP, int> pair = routeTable.lookup(destination);
		if (pair.second == -1)
			// do not know how to transfer
			return;
		// find the next hop
		IP ip = pair.first;
		// find the next hop mac
		MAC mac = this->arpTable.lookup(ip);
		if (mac.isBroadcast()) {
			((LinkLayer *) this->lowerLayers[pair.second])->sendARP(*ipConfiguration.getSegment(), ip);
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
			this->lowerLayers[pair.second]->send(newBlock);
		}
	}
}

void RouterNetworkLayer::sendDHCP() {
	this->sendDHCP0(true);
}

RouterNetworkLayer::~RouterNetworkLayer() {
	delete this->table;
}
