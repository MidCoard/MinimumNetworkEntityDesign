#include "LinkLayer.h"

LinkLayer::LinkLayer(NetworkEntity* networkEntity) : LinkLayer(0, networkEntity) {
}

std::string LinkLayer::getRawName() {
	return "LNK";
}


LinkLayer::LinkLayer(int id, NetworkEntity * networkEntity) : Layer(id, networkEntity) {

}

void LinkLayer::setMAC(int id, const MAC& mac) {
	idMacMap.insert({id, mac});
}

void LinkLayer::dealSend(Block* block) {
	if (block->getRemaining() < 6)
		return;
	auto* newBlock = new Block();
	MAC source = idMacMap.at(this->getID());
	newBlock->writeMAC(source);
	newBlock->writeBlock(block);
	newBlock->flip();
	this->lowerLayers[this->getID()]->send(newBlock);
}

// the id for PC this should always be 0
void LinkLayer::dealReceive(int id, Block* block) {
	if (block->getRemaining() < 12)
		return;
	MAC source = block->readMAC();
	MAC destination = block->readMAC();
	if (destination != idMacMap.at(id)) {
		return;
	} else {
		auto* newBlock = new Block();
		newBlock->writeBlock(block);
		newBlock->flip();
		this->upperLayers[this->getID()]->receive(this->getID(), newBlock);
	}
}
