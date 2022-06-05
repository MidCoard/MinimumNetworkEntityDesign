#include "LinkLayer.h"
#include "ARPPacket.h"
#include "NetworkLayer.h"
#include "ARPReplyPacket.h"

LinkLayer::LinkLayer(NetworkEntity *networkEntity) : LinkLayer(0, networkEntity) {
}

std::string LinkLayer::getRawName() {
	return "LNK";
}

LinkLayer::LinkLayer(int id, NetworkEntity *networkEntity) : Layer(id, networkEntity) {}

void LinkLayer::setMAC(int id, const MAC &mac) {
	idMacMap.insert({id, mac});
}

void LinkLayer::handleSend(Block *block) {
	if (block->getRemaining() < 6)
		return;
	auto *newBlock = new Block();
	MAC source = this->getMAC();
	newBlock->writeMAC(source);
	newBlock->writeBlock(block);
	newBlock->flip();
	auto *frame = new Frame(newBlock);
	delete newBlock;
	int size = frame->getSize();
	for (int i = 0; i < size; i++) {
		this->lowerLayers[0]->send(frame->createBlock(i));
	}
	delete frame;
}

// the id for PC this should always be 0
void LinkLayer::handleReceive(int id, Block *block0) {
	Block *block = this->frameTable.readFrame(block0);
	if (block == nullptr)
		return;
	if (block->getRemaining() < 12)
		return;
	MAC source = block->readMAC();
	MAC destination = block->readMAC();
	if (!destination.isBroadcast() && destination != this->idMacMap.at(id)) {
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
					this->log("Is it " + des.str() + "?");
					auto *networkLayer = (NetworkLayer *) this->upperLayers[0];
					if (networkLayer->isIPValid && des == networkLayer->getIP(id)) {
						this->log("Yes, I am " + des.str() + ", my MAC address is " + this->getMAC().str());
						this->sendARPReply(source, des, ip);
					}
				}
				break;
			case 0x87:
				if (this->upperLayers.size() == 1) {
					IP ip = block->readIP();
					IP des = block->readIP();
					this->log("I know " + ip.str() + " is " + source.str());
					auto *networkLayer = (NetworkLayer *) this->upperLayers[0];
					networkLayer->handleARP(ip, source);
				}
				break;
			default: {
				error("Unknown protocol type: " + std::to_string(header));
			}
		}
	}
	delete block;
}

void LinkLayer::sendARP(const IP &ip, const IP &query) {
	this->log("I am " + ip.str() + " ,I want to know who is " + query.str());
	auto *packet = new ARPPacket(ip, query);
	auto *block = packet->createBlock();
	delete packet;
	this->send(block);
}

void LinkLayer::sendARPReply(const MAC &mac, const IP &source, const IP &destination) {
	auto *packet = new ARPReplyPacket(source, destination, mac);
	auto *block = packet->createBlock();
	delete packet;
	this->send(block);
}

MAC LinkLayer::getMAC() {
	return this->idMacMap.at(this->getID());
}

