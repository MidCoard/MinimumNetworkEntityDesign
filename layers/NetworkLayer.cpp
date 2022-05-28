#include "NetworkLayer.h"

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


void NetworkLayer::dealReceive(int id, Block* block) {
	routeTable.check();
	if (!isIPValid)
		return;
	if (block->getRemaining() < 8)
		return;
	IP source = block->readIP();
	IP destination = block->readIP();
	// ip is valid so the ipConfiguration is valid
	IPConfiguration ipConfiguration = configurations.at(0);
	if (destination != *ipConfiguration.getSegment())
		return;
	else {
		auto *newBlock = new Block();
		newBlock->writeBlock(block);
		newBlock->flip();
		this->upperLayers[0]->receive(this->getID(), newBlock);
	}

}

void NetworkLayer::dealSend(Block* block) {
	routeTable.check();
	if (!isIPValid)
		return;
	if (block->getRemaining() < 4)
		return;
	IP destination = block->readIP();
	// ip is valid so the ipConfiguration is valid
	IPConfiguration ipConfiguration = configurations.at(0);
	// if PC route table has a route to the destination, send the block using the route
	int targetID = this->routeTable.lookup(destination);
	if (targetID != -1) {

		newBlock->writeBlock(block);
		this->lowerLayers[targetID]->send(newBlock);
	}
	else {
		// normally PC has no route to the destination, so send to the target or gateway
		// first check if the destination is in the same network

		if (destination.isInSameNetwork(*ipConfiguration.getSegment(), *ipConfiguration.getMask())) {
			// if the destination is in the same network, send to the target
			// get the mac address of the target
			MAC mac = this->arpTable.lookup(destination);
			// if the mac address is not found, send ARP, later send
			if (mac.isBroadcast()) {
				this->sendARP(destination);
			} else {
				// if the mac address is found, send the block
				auto * newBlock = new Block();
				newBlock->writeMAC(mac);
				newBlock->writeIP(*ipConfiguration.getSegment());
				newBlock->writeIP(destination);
				newBlock->writeBlock(block);
				newBlock->flip();
				this->lowerLayers[0]->send(newBlock);
			}
		}
		else {
			// if the destination is not in the same network, send to the gateway
			MAC mac = this->arpTable.lookup(*ipConfiguration.getGateway());
			// if the mac address is not found, send ARP, later send
			if (mac.isBroadcast()) {
				this->sendARP(destination);
			} else {
				// if the mac address is found, send the block
				auto * newBlock = new Block();
				newBlock->writeMAC(mac);
				newBlock->writeIP(*ipConfiguration.getSegment());
				newBlock->writeIP(destination);
				newBlock->writeBlock(block);
				newBlock->flip();
				this->lowerLayers[0]->send(newBlock);
			}
		}
	}
}
