#include "NetworkLayer.h"
#include "ARPPacket.h"
#include "ARPReplyPacket.h"
#include "DHCPDiscoverPacket.h"
#include "DHCPRequestPacket.h"
#include <utility>

NetworkLayer::NetworkLayer(NetworkEntity * networkEntity) : NetworkLayer(0, networkEntity) {}

std::string NetworkLayer::getRawName() {
	return "NET";
}

NetworkLayer::NetworkLayer(int id, NetworkEntity * networkEntity) : Layer(id, networkEntity) {}

void NetworkLayer::setIPConfiguration(int id, IP *segment, IP *mask, IP *gateway) {
	configurations.insert_or_assign(id, IPConfiguration(segment, mask, gateway));
}

IPConfiguration NetworkLayer::getIPConfiguration(int id) {
	return this->configurations.at(id);
}

unsigned long long NetworkLayer::size() {
	return this->configurations.size();
}

void NetworkLayer::handleReceive(int id, Block* block) {
	routeTable.check();
	if (block->getRemaining() < 8)
		return;
	IP source = block->readIP();
	IP destination = block->readIP();
	IPConfiguration ipConfiguration = configurations.at(0);
	if (!destination.isBroadcast() && (ipConfiguration.getSegment() == nullptr || destination != *ipConfiguration.getSegment()))
		return;
	else {
		unsigned char header;
		block->read(&header, 1);
		switch (header) {
			default:
				auto *newBlock = new Block();
				newBlock->writeBlock(block);
				newBlock->flip();
				this->upperLayers[0]->receive(this->getID(), newBlock);
				break;
		}
	}
}

void NetworkLayer::handleSend(Block* block) {
	routeTable.check();
	if (!isIPValid)
		return;
	auto now = std::chrono::system_clock::now().time_since_epoch().count();
	if (now - this->startDHCP > this->duration  * 3 / 4) // for 75%
		this->sendDHCPRenewal();
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
		// first check if the destination is in the same network
		if (nextHop.first.isInSameNetwork(*ipConfiguration.getSegment(), *ipConfiguration.getMask())) {
			// if the destination is in the same network, send to the target
			// get the mac address of the target
			MAC mac = this->arpTable.lookup(nextHop.first);
			// if the mac address is not found, send ARP, later send
			if (mac.isBroadcast()) {
				((LinkLayer*)this->lowerLayers[nextHop.second])->sendARP(*ipConfiguration.getSegment(),nextHop.first);
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				auto * newBlock = new Block(block->getSendCount() - 1);
				newBlock->writeIP(destination);
				newBlock->writeBlock(block);
				newBlock->flip();
				// wait for the ARP reply
				this->send(newBlock);
			} else {
				// if the mac address is found, send the block
				auto * newBlock = new Block();
				newBlock->writeMAC(mac);
				newBlock->write(0); // for ip protocol
				newBlock->writeIP(*ipConfiguration.getSegment());
				newBlock->writeIP(destination);
				newBlock->writeBlock(block);
				newBlock->flip();
				this->lowerLayers[nextHop.second]->send(newBlock);
			}
		}
	}
}

IP NetworkLayer::getIP() {
	if (!isIPValid)
		throw std::invalid_argument("IP is not valid");
	return *configurations.at(0).getSegment();
}

void NetworkLayer::handleARP(const IP& ip, const MAC& mac) {
	this->arpTable.update(ip, mac);
}

void NetworkLayer::sendDHCP0(bool useSegment) {
	IPConfiguration ipConfiguration = configurations.at(0);
	auto*linkLayer = (LinkLayer*)this->lowerLayers[0];
	if (ipConfiguration.getSegment() == nullptr || ipConfiguration.getMask() == nullptr) {
		auto *packet = new DHCPDiscoverPacket(linkLayer->getMAC(),useSegment);
		Block* block = packet->createBlock();
		delete packet;
		this->lowerLayers[0]->send(block);
	} else  {
		auto *packet = new DHCPRequestPacket(*ipConfiguration.getSegment(), *ipConfiguration.getMask(), linkLayer->getMAC(), this->dhcpID, useSegment);
		Block* block = packet->createBlock();
		delete packet;
		this->lowerLayers[0]->send(block);
	}
}


void NetworkLayer::sendDHCP() {
	this->sendDHCP0(false);
}

void NetworkLayer::sendDHCPRenewal0(bool useSegment) {
	if (this->isIPValid)
		return;
	IPConfiguration ipConfiguration = configurations.at(0);
	auto*linkLayer = (LinkLayer*)this->lowerLayers[0];
	auto *packet = new DHCPRequestPacket(*ipConfiguration.getSegment(), *ipConfiguration.getMask(), *ipConfiguration.getGateway(), linkLayer->getMAC(), useSegment);
	Block* block = packet->createBlock();
	delete packet;
	this->lowerLayers[0]->send(block);
}

void NetworkLayer::sendDHCPRenewal() {
	this->sendDHCPRenewal0(false);
}
