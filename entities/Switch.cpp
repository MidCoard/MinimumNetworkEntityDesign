#include "Switch.h"



Switch::Switch(Network*network, int node,std::map<int, SwitchConfiguration> switchConfigurations) : NetworkEntity(network, node, new LinkLayer()),switchConfigurations(std::move(switchConfigurations)) {}

std::vector<std::string> Switch::createLayers(int node, std::vector<int> ids) {
	for (int id: ids) {
		if (!this->switchConfigurations.empty()) {
			SwitchConfiguration switchConfiguration = this->switchConfigurations.at(id);
			if (switchConfiguration.getMAC() != nullptr)
				((LinkLayer*)this->layer)->setMAC(id, switchConfiguration.getMAC());
			else
				((LinkLayer*)this->layer)->setMAC(id, new MAC(generateMAC()));
			PhysicalLayer* physicalLayer;
			if (switchConfiguration.getPhysicalAddress() != nullptr)
				physicalLayer = new PhysicalLayer(id, switchConfiguration.getPhysicalAddress());
			else
				physicalLayer = new PhysicalLayer(id, new INetAddress(generatePhysicalAddress(node ,id)));
			this->layer->addLowerLayer(physicalLayer);
		} else {
			((LinkLayer*)this->layer)->setMAC(id, new MAC(generateMAC()));
			this->layer->addLowerLayer(new PhysicalLayer(id, new INetAddress(generatePhysicalAddress(node, id))));
		}
	}
	return this->layer->generateGraph(node);
}

SwitchConfiguration::SwitchConfiguration(MAC *mac, INetAddress *address) : mac(mac), address(address) {}

MAC *SwitchConfiguration::getMAC() {
	return this->mac;
}

INetAddress *SwitchConfiguration::getPhysicalAddress() {
	return this->address;
}
