//
// Created by 周蜀杰 on 2022/6/8.
//

#include "HomeRouter.h"
#include "DHCPOfferPacket.h"
#include "DHCPACKPacket.h"
#include "DHCPNAKPacket.h"
#include "DHCPDeclinePacket.h"
#include "ICMPPacket.h"

#include <utility>

void HomeRouterNetworkLayer::handleReceive(int id, Block *block) {
	this->checkDHCP();
	if (block->getSendCount() < 0)
		return;
	if (block->getRemaining() < 8)
		return;
	IP source = block->readIP();
	IP destination = block->readIP();
	IPConfiguration wanIPConfiguration = this->getIPConfiguration(0);
	if (destination.isBroadcast()) {
		if (!this->isIPValid)
			return;
		unsigned char header;
		block->read(&header, 1);
		switch (header) {
			case 0x01:
				if (id != 0 && source == LOCAL0) {
					MAC mac = block->readMAC();
					unsigned char useSegment;
					block->read(&useSegment, 1);
					log("Receive DHCP discover from " + mac.str() + " useSegment: " + std::to_string(useSegment));
					if (useSegment) {
						throw std::invalid_argument("unsupported");
					} else {
						std::pair<IP, long long int> app = this->tables[id]->apply();
						if (app.second != -1) {
							auto *packet = new DHCPOfferPacket(mac, app.first, PRIVATE_NETWORK_MASK,
							                                   PRIVATE_NETWORK_IP, app.second);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						}
					}
				}
				break;
			case 0x03:
				if (id != 0 && source == LOCAL0) {
					IP segment = block->readIP();
					IP mask = block->readIP();
					MAC mac = block->readMAC();
					int dhcpID = block->readInt();
					unsigned char useSegment;
					block->read(&useSegment, 1);
					log("DHCP request from with segment " + segment.str() + " and mask " + mask.str() + " and mac " +
					    mac.str() + " and dhcpID " + std::to_string(dhcpID));
					if (useSegment) {
						throw std::invalid_argument("unsupported");
					} else {
						auto pair = this->natTable->apply();
						if (pair.second != -1 && this->tables[id]->applyItOrMAC(segment, mac, dhcpID)) {
							auto *packet = new DHCPACKPacket(mac, PRIVATE_NETWORK_IP, segment, mask,
							                                 PRIVATE_NETWORK_IP,
							                                 kDHCPTime);
							if (!this->natTable->applyIt(pair.first,mac, pair.second))
								throw std::invalid_argument("ack failed");
							this->natMap.insert_or_assign(segment, pair.first);
							this->natMapReverse.insert_or_assign(pair.first, segment);
							this->routeTable.updateLong(segment, PRIVATE_NETWORK_MASK, 0, segment, id);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						} else {
							this->tables[id]->print();
							auto *packet = new DHCPNAKPacket(mac, PRIVATE_NETWORK_IP);
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
						throw std::invalid_argument("unsupported");
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
		unsigned char header;
		block->read(&header, 1);
		switch (header) {
			case 0x02:
				if (id == 0) {// receive dhcp offer
					if (wanIPConfiguration.isValid())
						return;
					IP segment = block->readIP();
					IP mask = block->readIP();
					IP gateway = block->readIP();
					auto *linkLayer = (LinkLayer *) this->lowerLayers[id];
					MAC mac = this->arpTable.lookup(segment);
					int count = 0;
					this->offerable = false;
					while (mac.isBroadcast() && count < 5) {
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
						delete wanIPConfiguration.getSegment();
						delete wanIPConfiguration.getMask();
						delete wanIPConfiguration.getGateway();
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
					delete wanIPConfiguration.getSegment();
					delete wanIPConfiguration.getMask();
					delete wanIPConfiguration.getGateway();
					this->setIPConfiguration(0, new IP(segment), new IP(mask), new IP(gateway));
					this->startDHCP = util::getNowTime();
					this->duration = block->readLong();
					this->isIPValid = true;
					for (auto &table: this->tables)
						delete table;
					this->tables.clear();
					this->tables.push_back(new DHCPTable(PRIVATE_NETWORK_IP, PRIVATE_NETWORK_MASK));
					auto *layer = (LinkLayer *) this->lowerLayers[0];
					this->natTable = new DHCPTable(segment, mask);
					bool flag = this->tables[0]->directApply(PRIVATE_NETWORK_IP, layer->getMAC());
					if (!flag)
						throw std::runtime_error("apply its segment failed");
					this->routeTable.updateLong(LOCAL0, LOCAL0, 10, gateway, 0);
				}
				break;
			case 0x05:
				if (id == 0) {
					// receive dhcp nak
					this->dhcpID = -1;
					delete wanIPConfiguration.getSegment();
					delete wanIPConfiguration.getMask();
					delete wanIPConfiguration.getGateway();
					this->setIPConfiguration(0, nullptr, nullptr, nullptr);
				}
				break;
			default: {
				error("HomeRouter is not available");
			}
		}
	} else if (destination == PRIVATE_NETWORK_IP) {
		// receive packet from private network
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
						throw std::invalid_argument("unsupported");
					} else {
						if (this->tables[id]->renewal(segment, mac)) {
							auto *packet = new DHCPACKPacket(mac, PRIVATE_NETWORK_IP, segment, mask,
							                                 PRIVATE_NETWORK_IP,
							                                 kDHCPTime);
							if (!this->natTable->renewal(natMap.at(segment),mac))
								throw std::runtime_error("renewal failed");
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						} else {
							auto *packet = new DHCPNAKPacket(mac, PRIVATE_NETWORK_IP);
							auto *newBlock = packet->createBlock();
							delete packet;
							this->lowerLayers[id]->send(newBlock);
						}
					}
				}
				break;
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
					this->natTable->release(natMap.at(segment), *wanIPConfiguration.getMask(), mac,useSegment);
					natMapReverse.erase(natMap.at(segment));
					natMap.erase(segment);
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
	} else if (destination == *wanIPConfiguration.getSegment()) {
		unsigned char header;
		block->read(&header, 1);
		switch (header) {
			case 0x04:
				if (id == 0) {
					IP segment = block->readIP();
					IP mask = block->readIP();
					IP gateway = block->readIP();
					this->log("receive DHCP_ACK get segment: " + segment.str() + " mask: " + mask.str() +
					          " gateway: " + gateway.str());
					delete wanIPConfiguration.getSegment();
					delete wanIPConfiguration.getMask();
					delete wanIPConfiguration.getGateway();
					this->setIPConfiguration(0, new IP(segment), new IP(mask), new IP(gateway));
					this->startDHCP = util::getNowTime();
					this->duration = block->readLong();
					this->isIPValid = true;
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
			default:{
				error("Unknown protocol type " + std::to_string(header) + " from " + source.str() + " to " +
				      destination.str());
			}
		}
	} else {
		// transfer
		bool flag = false;
		if (this->natMapReverse.find(destination) == this->natMapReverse.end()) {
			// ready to packet and transfer
			if (this->natMap.find(source) != this->natMap.end()) {
				flag = true;
			} else return;
		} else destination = this->natMapReverse.at(destination);
		std::pair<IP, int> nextHop = routeTable.lookup(destination);
		if (nextHop.second == -1)
			// do not know how to transfer
			return;
		IP ip = nextHop.first;
		MAC mac = this->arpTable.lookup(ip);
		if (mac.isBroadcast()) {
			((LinkLayer *) this->lowerLayers[nextHop.second])->sendARP(
					nextHop.second != 0 ? PRIVATE_NETWORK_IP : *wanIPConfiguration.getSegment(), ip);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			auto *newBlock = new Block(block->getSendCount() - 1);
			newBlock->writeIP(source);
			newBlock->writeIP(destination);
			newBlock->writeBlock(block);
			newBlock->flip();
			this->receive(id, newBlock);
		} else {
			if (flag) {
				auto* newBlock = new Block();
				newBlock->writeMAC(mac);
				newBlock->write(0);
				newBlock->writeIP(this->natMap.at(source));
				newBlock->writeIP(destination);
				newBlock->writeBlock(block);
				newBlock->flip();
				this->lowerLayers[nextHop.second]->send(newBlock);
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

HomeRouterNetworkLayer::HomeRouterNetworkLayer(NetworkEntity *entity) : RouterNetworkLayer(entity) {}

void HomeRouterNetworkLayer::handleSend(Block *block) {
	this->checkDHCP();
	if (!isIPValid)
		return;
	if (block->getSendCount() < 0)
		return;
	if (block->getRemaining() < 4)
		return;
	IP destination = block->readIP();
	IPConfiguration ipConfiguration = configurations.at(0);
	std::pair<IP, int> nextHop = this->routeTable.lookup(destination);
	if (nextHop.second != -1) {
		// normally PC has no route to the destination, so send to the target or gateway
		// if the destination is in the same network, send to the target
		// get the mac address of the target
		IPConfiguration configuration = this->configurations.at(nextHop.second);
		if (nextHop.first == *configuration.getSegment() && nextHop.first == destination) {
			auto *newBlock = new Block();
			newBlock->writeIP(destination);
			newBlock->writeIP(destination);
			newBlock->writeBlock(block);
			newBlock->flip();
			this->receive(nextHop.second, newBlock);
		} else {
			IP ip = nextHop.first == *configuration.getSegment() ? destination : nextHop.first;
			MAC mac = this->arpTable.lookup(ip);
			// if the mac address is not found, send ARP, later send
			if (mac.isBroadcast()) {
				// won't happen configuration.getSegment() == nextHop.first
				((LinkLayer *) this->lowerLayers[nextHop.second])->sendARP(*configuration.getSegment(),
				                                                           ip);
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				auto *newBlock = new Block(block->getSendCount() - 1);
				newBlock->writeIP(destination);
				newBlock->writeBlock(block);
				newBlock->flip();
				// wait for the ARP reply
				this->send(newBlock);
			} else {
				// if the mac address is found, send the block
				auto *newBlock = new Block();
				newBlock->writeMAC(mac);
				newBlock->write(0); // for ip protocol
				newBlock->writeIP(destination.isInPrivateNetwork() ? PRIVATE_NETWORK_IP : *configuration.getSegment());
				newBlock->writeIP(destination);
				newBlock->writeBlock(block);
				newBlock->flip();
				this->lowerLayers[nextHop.second]->send(newBlock);
			}
		}
	} else if (destination.isInSameNetwork(*ipConfiguration.getSegment(), *ipConfiguration.getMask())) {
		// special case the destination is in the same network
		MAC mac = this->arpTable.lookup(destination);
		if (mac.isBroadcast()) {
			// won't happen destination == ipConfiguration.getSegment()
			((LinkLayer *) this->lowerLayers[0])->sendARP(*ipConfiguration.getSegment(), destination);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			auto *newBlock = new Block(block->getSendCount() - 1);
			newBlock->writeIP(destination);
			newBlock->writeBlock(block);
			newBlock->flip();
			this->send(newBlock);
		} else {
			auto *newBlock = new Block();
			newBlock->writeMAC(mac);
			newBlock->write(0); // for ip protocol
			newBlock->writeIP(destination.isInPrivateNetwork() ? PRIVATE_NETWORK_IP : *ipConfiguration.getSegment());
			newBlock->writeIP(destination);
			newBlock->writeBlock(block);
			newBlock->flip();
			this->lowerLayers[0]->send(newBlock);
		}

	} else
		error("Don't know how to send the packet");
}


std::string HomeRouter::getName() {
	return "HomeRouter";
}

HomeRouter::HomeRouter(Network *network, int node, std::map<int, RouterConfiguration *> map) : Router(network, node,
                                                                                                      std::move(map),
                                                                                                      new HomeRouterNetworkLayer(
		                                                                                                      this)) {}

void HomeRouter::generateIP() {
	if (generatedIP)
		return;
	this->generatedIP = true;
}

std::vector<IPConfiguration> HomeRouter::getIPConfiguration() {
	generateIP();
	IPConfiguration ipConfiguration = ((HomeRouterNetworkLayer *) this->layer)->getIPConfiguration(0);
	if (ipConfiguration.isConfigurable())
		return {ipConfiguration};
	return {};
}

std::vector<std::string> HomeRouter::createLayers(int node, std::vector<int> ids) {
	RouterConfiguration *routerConfiguration = routerConfigurations.at(0);
	((RouterNetworkLayer *) this->layer)->setIPConfiguration(0, routerConfiguration->getSegment(),
	                                                         routerConfiguration->getMask(),
	                                                         routerConfiguration->getGateway());
	return Router::createLayers(node, ids);
}
