#include "Switch.h"


Switch::Switch(Network *network, int node, std::map<int, SwitchConfiguration *> switchConfigurations) : NetworkEntity(
		network, node, new SwitchLinkLayer(this)), switchConfigurations(std::move(switchConfigurations)) {}

std::vector<std::string> Switch::createLayers(int node, std::vector<int> ids) {
	for (int id: ids) {
		if (!this->switchConfigurations.empty()) {
			SwitchConfiguration *switchConfiguration = this->switchConfigurations.at(id);
			auto *physicalLayer = new PhysicalLayer(id, this,
			                                        switchConfiguration->getLinkAddress() == nullptr
			                                        ? generateLinkAddress(node, id)
			                                        : *switchConfiguration->getLinkAddress(),
			                                        switchConfiguration->getPhysicalAddress() == nullptr
			                                        ? generatePhysicalAddress(node, id)
			                                        : *switchConfiguration->getPhysicalAddress());
			this->layer->addLowerLayer(physicalLayer);
		} else
			this->layer->addLowerLayer(
					new PhysicalLayer(id, this, generateLinkAddress(node, id), generatePhysicalAddress(node, id)));
	}
	return this->layer->generateGraph(node);
}

Switch::~Switch() {
	for (const auto &[key, value]: switchConfigurations)
		delete value;
}

std::string Switch::getName() {
	return "Switch";
}

SwitchConfiguration::SwitchConfiguration(INetAddress *linkAddress, INetAddress *physicalAddress)
		: linkAddress(linkAddress), physicalAddress(physicalAddress) {}


INetAddress *SwitchConfiguration::getPhysicalAddress() {
	return this->physicalAddress;
}

INetAddress *SwitchConfiguration::getLinkAddress() {
	return this->linkAddress;
}

SwitchConfiguration::~SwitchConfiguration() {
	delete this->linkAddress;
	delete this->physicalAddress;
}

SwitchLinkLayer::SwitchLinkLayer(NetworkEntity *networkEntity) : LinkLayer(networkEntity) {}

void SwitchLinkLayer::handleReceive(int id, Block *block0) {
	Block *block = this->frameTable.readFrame(block0);
	if (block == nullptr)
		return;
	if (block->getRemaining() < 12)
		return;
	MAC source = block->readMAC();
	MAC destination = block->readMAC();
	macTable.update(source, id);
	auto *newBlock = new Block();
	newBlock->writeMAC(source);
	newBlock->writeMAC(destination);
	newBlock->writeBlock(block);
	newBlock->flip();
	if (destination.isBroadcast()) {
		for (auto layer: this->lowerLayers)
			if (layer->getID() != id) {
				auto *b = newBlock->copy();
				auto *frame = new Frame(b);
				delete b;
				int size = frame->getSize();
				for (int i = 0; i < size; i++) {
					layer->send(frame->createBlock(i));
				}
				this->frameTable.add(frame);
			}
	} else {
		int interface = macTable.lookup(destination);
		if (interface == -1) {
			for (auto layer: this->lowerLayers)
				if (layer->getID() != id) {
					auto *b = newBlock->copy();
					auto *frame = new Frame(b);
					delete b;
					int size = frame->getSize();
					for (int i = 0; i < size; i++) {
						layer->send(frame->createBlock(i));
					}
					this->frameTable.add(frame);
				}
		} else {
			auto *b = newBlock->copy();
			auto *frame = new Frame(b);
			delete b;
			int size = frame->getSize();
			for (int i = 0; i < size; i++) {
				this->lowerLayers.at(interface)->send(frame->createBlock(i));
			}
			this->frameTable.add(frame);
		}
	}
	delete newBlock;
	delete block;
}

void SwitchLinkLayer::handleSend(Block *block) {
	// do nothing
}
