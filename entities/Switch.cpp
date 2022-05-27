#include "Switch.h"



Switch::Switch(Network*network, int node,std::map<int, SwitchConfiguration> switchConfigurations) : NetworkEntity(network, node, new LinkLayer(this)),switchConfigurations(std::move(switchConfigurations)) {}

std::vector<std::string> Switch::createLayers(int node, std::vector<int> ids) {
	for (int id: ids) {
		if (!this->switchConfigurations.empty()) {
			SwitchConfiguration switchConfiguration = this->switchConfigurations.at(id);
			if (switchConfiguration.getMAC() != nullptr)
				((LinkLayer*)this->layer)->setMAC(id, *switchConfiguration.getMAC());
			else
				((LinkLayer*)this->layer)->setMAC(id, generateMAC());
			auto* physicalLayer = new PhysicalLayer(id,this,
															 switchConfiguration.getLinkAddress() == nullptr ? generateLinkAddress(node ,id) : *switchConfiguration.getLinkAddress(),
															 switchConfiguration.getPhysicalAddress() == nullptr ? generatePhysicalAddress(node ,id) : *switchConfiguration.getPhysicalAddress());
			this->layer->addLowerLayer(physicalLayer);
		} else {
			((LinkLayer*)this->layer)->setMAC(id, generateMAC());
			this->layer->addLowerLayer(new PhysicalLayer(id,this, generateLinkAddress(node ,id), generatePhysicalAddress(node, id)));
		}
	}
	return this->layer->generateGraph(node);
}

SwitchConfiguration::SwitchConfiguration(MAC *mac, INetAddress *linkAddress, INetAddress *physicalAddress)
		: mac(mac), linkAddress(linkAddress), physicalAddress(physicalAddress) {}

MAC *SwitchConfiguration::getMAC() {
	return this->mac;
}

INetAddress *SwitchConfiguration::getPhysicalAddress() {
	return this->physicalAddress;
}

INetAddress *SwitchConfiguration::getLinkAddress() {
	return this->linkAddress;
}

SwitchConfiguration::~SwitchConfiguration() {
	delete this->mac;
	delete this->linkAddress;
	delete this->physicalAddress;
}
