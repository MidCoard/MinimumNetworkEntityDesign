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

void SwitchLinkLayer::handleReceive(int id, Block *block) {
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
				layer->send(newBlock->copy());
			}
	} else {
		int interface = macTable.lookup(destination);
		if (interface == -1) {
			for (auto layer: this->lowerLayers)
				if (layer->getID() != id) {
					layer->send(newBlock->copy());
				}
		} else
			this->lowerLayers.at(interface)->send(newBlock->copy());
	}
	delete newBlock;
}

void SwitchLinkLayer::handleSend(Block *block) {
	// do nothing
}

void SwitchLinkLayer::receive(int id, Block *block) {
	debug("from interface " + std::to_string(id) + " size " + std::to_string(block->size()));
	Layer::receive(id, block);
}

