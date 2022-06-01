#include "NetworkLayer.h"
#include "ARPPacket.h"
#include "DHCPDiscoverPacket.h"
#include "DHCPRequestPacket.h"
#include "PC.h"
#include "DHCPDeclinePacket.h"
#include "ICMPPacket.h"
#include "DHCPReleasePacket.h"
#include "DHCPHelper.h"
#include <utility>

NetworkLayer::NetworkLayer(NetworkEntity *networkEntity) : NetworkLayer(0, networkEntity) {}

std::string NetworkLayer::getRawName() {
	return "NET";
}

NetworkLayer::NetworkLayer(int id, NetworkEntity *networkEntity) : Layer(id, networkEntity) {}

void NetworkLayer::setIPConfiguration(int id, IP *segment, IP *mask, IP *gateway) {
	configurations.insert_or_assign(id, IPConfiguration(segment, mask, gateway));
}

IPConfiguration NetworkLayer::getIPConfiguration(int id) {
	return this->configurations.at(id);
}

unsigned long long NetworkLayer::size() {
	return this->configurations.size();
}

void NetworkLayer::handleReceive(int id, Block *block) {
	this->checkDHCP();
	if (id != 0)
		this->error("PC must receive on interface 0");
	if (block->getRemaining() < 8)
		return;
	IP source = block->readIP();
	IP destination = block->readIP();
	IPConfiguration ipConfiguration = configurations.at(0);
	if (destination.isBroadcast() && isIPValid) {
		auto *newBlock = new Block();
		newBlock->writeBlock(block);
		newBlock->flip();
		this->upperLayers[0]->receive(this->getID(), newBlock);
	} else if (isIPValid && *ipConfiguration.getSegment() == destination) {
		unsigned char header;
		block->read(&header, 1);
		switch (header) {
			case 0x00: {
				if (this->upperLayers.empty())
					this->error("No upper layer");
				else {
					this->upperLayers[0]->receive(id,new Block(block));
				}
				break;
			}
			case 0x04: {
				IP ip = block->readIP();
				IP mask = block->readIP();
				IP gateway = block->readIP();
				this->log("receive DHCP_ACK get segment: " + ip.str() + " mask: " + mask.str() + " gateway: " +
				          gateway.str());
				auto *pc = (PC *) this->networkEntity;
				delete pc->ip;
				delete pc->mask;
				delete pc->gateway;
				pc->ip = new IP(ip);
				pc->mask = new IP(mask);
				pc->gateway = new IP(gateway);
				this->setIPConfiguration(0, pc->ip, pc->mask, pc->gateway);
				this->startDHCP = std::chrono::system_clock::now().time_since_epoch().count();
				this->duration = block->readLong();
				this->isIPValid = true;
				this->routeTable.updateShort(LOCAL0,LOCAL0,10,gateway,0);
				break;
			}
			case 0x20: {
				IP ip = block->readIP();
				IP query = block->readIP();
				if (query == *ipConfiguration.getSegment()) {
					auto *packet = new ICMPReplyPacket(ip,query,source,ICMPReplyStatus::kICMPReplyStatusSuccess);
					auto* newBlock = packet->createBlock();
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
					this->handleICMP(query, flag ? ICMPReplyStatus::kICMPReplyStatusUnreachable
					                             : ICMPReplyStatus::kICMPReplyStatusSuccess);
				}
				break;
			}
			default: {
				error("Unknown protocol type " + std::to_string(header) + " from " + source.str() + " to " + destination.str());
			}
		}
	} else if (!isIPValid) {
		unsigned char header;
		block->read(&header, 1);
		switch (header) {
			case 0x02: {
				if (ipConfiguration.isValid())
					return;
				// handle dhcp offer
				IP ip = block->readIP();
				IP mask = block->readIP();
				IP gateway = block->readIP();
				auto *linkLayer = (LinkLayer *) this->lowerLayers[id];
				linkLayer->sendARP(ip, ip);
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));
				MAC mac = this->arpTable.lookup(ip);
				if (!mac.isBroadcast()) {
					this->error("DHCP offer a address that has been used");
					// one have already got the ip (maybe static ip)
					auto *packet = new DHCPDeclinePacket(ip, mask, mac, false);
					auto *newBlock = packet->createBlock();
					delete packet;
					this->lowerLayers[id]->send(newBlock);
				} else {
					this->dhcpID = block->readInt();
					auto *pc = (PC *) this->networkEntity;
					delete pc->ip;
					delete pc->mask;
					delete pc->gateway;
					pc->ip = new IP(ip);
					pc->mask = new IP(mask);
					pc->gateway = new IP(gateway);
					this->setIPConfiguration(0, pc->ip, pc->mask, pc->gateway);
				}
				break;
			}
			case 0x04: {
				IP ip = block->readIP();
				IP mask = block->readIP();
				IP gateway = block->readIP();
				this->log("receive DHCP_ACK get segment: " + ip.str() + " mask: " + mask.str() + " gateway: " +
				          gateway.str());
				auto *pc = (PC *) this->networkEntity;
				delete pc->ip;
				delete pc->mask;
				delete pc->gateway;
				pc->ip = new IP(ip);
				pc->mask = new IP(mask);
				pc->gateway = new IP(gateway);
				this->setIPConfiguration(0, pc->ip, pc->mask, pc->gateway);
				this->startDHCP = std::chrono::system_clock::now().time_since_epoch().count();
				this->duration = block->readLong();
				this->isIPValid = true;
				break;
			}
			case 0x05: {
				// receive dhcp nak
				this->dhcpID = -1;
				delete ipConfiguration.getSegment();
				delete ipConfiguration.getMask();
				delete ipConfiguration.getGateway();
				auto *pc = (PC *) this->networkEntity;
				pc->ip = nullptr;
				pc->mask = nullptr;
				pc->gateway = nullptr;
				this->setIPConfiguration(0, nullptr, nullptr, nullptr);
				break;
			}
			default: {
				error("PC is not available");
			}
		}
	} else {
		error("Discard packet");
	}
}

void NetworkLayer::handleSend(Block *block) {
	this->checkDHCP();
	if (!isIPValid)
		return;
	if (block->getSendCount() < 0)
		return;
	if (block->getRemaining() < 4)
		return;
	IP destination = block->readIP();
	// ip is valid so the ipConfiguration is valid
	IPConfiguration ipConfiguration = configurations.at(0);
	// if PC route table has a route to the destination, send the block using the route
	std::pair<IP, int> nextHop = this->routeTable.lookup(destination);
	if (nextHop.second != -1) {
		// normally PC has no route to the destination, so send to the target or gateway
		// if the destination is in the same network, send to the target
		// get the mac address of the target
		IPConfiguration configuration = this->configurations.at(nextHop.second);
		if (nextHop.first == *configuration.getSegment() && nextHop.first == destination) {
			auto* newBlock = new Block();
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
				newBlock->writeIP(*configuration.getSegment());
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
			newBlock->writeIP(*ipConfiguration.getSegment());
			newBlock->writeIP(destination);
			newBlock->writeBlock(block);
			newBlock->flip();
			this->lowerLayers[0]->send(newBlock);
		}

	} else
		error("Don't know how to send the packet");
}

IP NetworkLayer::getIP(int id) {
	if (!isIPValid)
		throw std::invalid_argument("IP is not valid");
	return *configurations.at(id).getSegment();
}

void NetworkLayer::handleARP(const IP &ip, const MAC &mac) {
	this->arpTable.update(ip, mac);
}

void NetworkLayer::sendDHCP0(bool useSegment) {
	IPConfiguration ipConfiguration = configurations.at(0);
	auto *linkLayer = (LinkLayer *) this->lowerLayers[0];
	if (!ipConfiguration.isConfigurable()) {
		auto *packet = new DHCPDiscoverPacket(linkLayer->getMAC(), useSegment);
		Block *block = packet->createBlock();
		delete packet;
		this->lowerLayers[0]->send(block);
	} else {
		log("Send DHCP Request with segment " + ipConfiguration.getSegment()->str() + " and mask " + ipConfiguration.getMask()->str() + " and mac " + linkLayer->getMAC().str() + " and useSegment " + std::to_string(useSegment));
		auto *packet = new DHCPRequestPacket(*ipConfiguration.getSegment(), *ipConfiguration.getMask(),
		                                     linkLayer->getMAC(), this->dhcpID, useSegment);
		Block *block = packet->createBlock();
		delete packet;
		this->lowerLayers[0]->send(block);
	}
}


void NetworkLayer::sendDHCP() {
	this->sendDHCP0(false);
}

void NetworkLayer::sendDHCPRenewal0(bool useSegment) {
	if (!this->isIPValid)
		return;
	if (!renewable)
		return;
	renewable = false;
	IPConfiguration ipConfiguration = configurations.at(0);
	auto *linkLayer = (LinkLayer *) this->lowerLayers[0];
	MAC target = this->arpTable.lookup(*ipConfiguration.getGateway());
	this->log("Ready to send DHCP Renewal with segment " + ipConfiguration.getSegment()->str() + " and mask " + ipConfiguration.getMask()->str() + " and mac " + linkLayer->getMAC().str() + " and useSegment " + std::to_string(useSegment));
	if (target.isBroadcast()) {
		linkLayer->sendARP(*ipConfiguration.getSegment(), *ipConfiguration.getGateway());
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	target = this->arpTable.lookup(*ipConfiguration.getGateway());
	if (target.isBroadcast()) {
		this->error("Send DHCP Renewal failed. Cannot find gateway MAC");
		renewable = true;
		return;
	}
	kExecutor.submit([this]() {
		this->renewable = true;
	},std::chrono::seconds(1));
	this->log("Send DHCP Renewal with segment " + ipConfiguration.getSegment()->str() + " and mask " + ipConfiguration.getMask()->str() + " and mac " + linkLayer->getMAC().str() + " and useSegment " + std::to_string(useSegment));
	auto *packet = new DHCPRequestPacket(*ipConfiguration.getSegment(), *ipConfiguration.getMask(),
	                                     *ipConfiguration.getGateway(), linkLayer->getMAC(), target, useSegment);
	Block *block = packet->createBlock();
	delete packet;
	linkLayer->send(block);
}

void NetworkLayer::sendDHCPRenewal() {
	this->sendDHCPRenewal0(false);
}

void NetworkLayer::checkDHCP() {
	if (this->startDHCP == 0 && this->duration == 0)
		return;
	auto now = std::chrono::system_clock::now().time_since_epoch().count();
	if (now > this->startDHCP + this->duration) {
		this->isIPValid = false;
		this->startDHCP = 0;
		this->duration = 0;
		this->dhcpID = -1;
		dhcp::request(this);
		return;
	}
	if (now - this->startDHCP > this->duration * 3 / 4) // for 75%
		this->sendDHCPRenewal();
}

void NetworkLayer::sendICMP(IP ip) {
	IPConfiguration ipConfiguration = configurations.at(0);
	if (ipConfiguration.getSegment() == nullptr || ipConfiguration.getGateway() == nullptr)
		return;
	IP segment = *ipConfiguration.getSegment();
	kExecutor.submit([this,segment,ip]() {
		if (this->icmpTable.lookupAndUpdate(segment,ip, true) != -1) {
			this->handleICMP(ip, ICMPReplyStatus::kICMPReplyStatusUnreachable);
		}
	},std::chrono::seconds(5));
	this->icmpTable.add(*ipConfiguration.getSegment(), ip);
	auto *packet = new ICMPPacket(segment, std::move(ip), *ipConfiguration.getGateway());
	Block *block = packet->createBlock();
	delete packet;
	this->send(block);
}

void NetworkLayer::handleICMP(const IP &ip, ICMPReplyStatus status) {
	if (status == ICMPReplyStatus::kICMPReplyStatusUnreachable)
		error("IP " + ip.str() + " is unreachable");
	else log("IP " + ip.str() + " is reachable");
}

void NetworkLayer::sendDHCPRelease() {
	this->sendDHCPRelease0(false);
}

void NetworkLayer::sendDHCPRelease0(bool useSegment) {
	if (!this->isIPValid) {
		error("IP is not valid, no need to send DHCP release");
		return;
	}
	if (this->startDHCP == 0 && this->duration == 0) {
		error("IP is static, no need to send DHCP release");
		return;
	}
	this->isIPValid = false;
	this->startDHCP = 0;
	this->duration = 0;
	this->dhcpID = -1;
	IPConfiguration ipConfiguration = this->getIPConfiguration(0);
	auto * linkLayer = (LinkLayer*)this->lowerLayers[0];
	MAC mac = this->arpTable.lookup(*ipConfiguration.getGateway());
	if (mac.isBroadcast()) {
		linkLayer->sendARP(*ipConfiguration.getSegment(), *ipConfiguration.getGateway());
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	mac = this->arpTable.lookup(*ipConfiguration.getGateway());
	if (mac.isBroadcast()) {
		this->error("Send DHCP Release failed. Cannot find gateway MAC");
		return;
	}
	auto* packet = new DHCPReleasePacket(*ipConfiguration.getSegment(),*ipConfiguration.getMask(),*ipConfiguration.getGateway(),mac,linkLayer->getMAC(),useSegment);
	auto* block = packet->createBlock();
	delete packet;
	linkLayer->send(block);
}

