#include "LinkLayer.h"
#include "ARPPacket.h"
#include "NetworkLayer.h"
#include "ARPReplyPacket.h"

LinkLayer::LinkLayer(NetworkEntity* networkEntity) : LinkLayer(0, networkEntity) {
}

std::string LinkLayer::getRawName() {
	return "LNK";
}

LinkLayer::LinkLayer(int id, NetworkEntity * networkEntity) : Layer(id, networkEntity) {}

void LinkLayer::setMAC(int id, const MAC& mac) {
	idMacMap.insert({id, mac});
}

void LinkLayer::handleSend(Block* block) {
	if (block->getRemaining() < 6)
		return;
	auto* newBlock = new Block();
	MAC source = this->getMAC();
	newBlock->writeMAC(source);
	newBlock->writeBlock(block);
	newBlock->flip();
	this->lowerLayers[0]->send(newBlock);
}

// the id for PC this should always be 0
void LinkLayer::handleReceive(int id, Block* block) {
	if (block->getRemaining() < 12)
		return;
	MAC source = block->readMAC();
	MAC destination = block->readMAC();
	if (!destination.isBroadcast() && destination != this->idMacMap.at(0)) {
		return;
	} else {
		unsigned char header;
		block->read(&header, 1);
		// link protocol
		switch (header) {
			case 0x00: {
				auto *newBlock = new Block();
				newBlock->writeBlock(block);
				newBlock->flip();
				this->upperLayers[0]->receive(this->getID(), newBlock);
				break;
			}
			case 0x86:
				if (this->upperLayers.size() == 1) {
					IP ip = block->readIP();
					IP des = block->readIP();
					auto* networkLayer = (NetworkLayer*) this->upperLayers[0];
					if (networkLayer->isIPValid && des == networkLayer->getIP())
						this->sendARPReply(id, source, des, ip);
				}
				break;
			case 0x87:
				if (this->upperLayers.size() == 1) {
					IP ip = block->readIP();
					IP des = block->readIP();
					auto* networkLayer = (NetworkLayer*) this->upperLayers[0];
					if (networkLayer->isIPValid && des == networkLayer->getIP())
						networkLayer->handleARP(ip, source);
				}
		}
	}
}

void LinkLayer::sendARP(const IP& ip, const IP& query) {
	auto * packet = new ARPPacket(ip, query);
	auto * block = packet->createBlock();
	delete packet;
	this->send(block);
}

void LinkLayer::sendARPReply(int id, const MAC& mac, const IP& source, const IP& destination) {
	auto * packet = new ARPReplyPacket(source, destination, mac);
	auto * block = packet->createBlock();
	delete packet;
	this->lowerLayers[id]->send(block);
}

MAC LinkLayer::getMAC() {
	return this->idMacMap.at(this->getID());
}

