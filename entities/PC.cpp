#include "PC.h"

// why here using pointer to IP, MAC etc.
// to present null IP, MAC etc.
// all should be deleted in layers not in the entity
PC::PC(Network *network, int node, IP *ip, IP *mask, IP *gateway, MAC *mac, INetAddress *linkAddress,
       INetAddress *physicalAddress)
		: NetworkEntity(network, node, new AppLayer()), ip(ip), mask(mask), gateway(gateway), mac(mac),linkAddress(linkAddress), physicalAddress(physicalAddress) {}


std::vector<std::string> PC::createLayers(int node, std::vector<int> ids) {
	if (ids.size() != 1)
		throw std::invalid_argument("PC network interface size should be 1");
	auto *networkLayer = new NetworkLayer(ids[0]);
	networkLayer->setIPConfiguration(ids[0], new IP(*this->ip), new IP(*this->mask), new IP(*this->gateway));
	this->layer->addLowerLayer(networkLayer);
	auto *linkLayer = new LinkLayer(ids[0]);
	if (mac == nullptr)
		mac = new MAC(generateMAC());
	linkLayer->setMAC(ids[0], *mac);
	networkLayer->addLowerLayer(linkLayer);
	if (linkAddress == nullptr)
		linkAddress = new INetAddress(generateLinkAddress(node, ids[0]));
	if (physicalAddress == nullptr)
		physicalAddress = new INetAddress(generatePhysicalAddress(node, ids[0]));
	auto *physicalLayer = new PhysicalLayer(ids[0], *linkAddress, *physicalAddress);
	linkLayer->addLowerLayer(physicalLayer);
	return this->layer->generateGraph(node);
}

bool PC::isIPAvailable() {
	return true;
}

std::vector<IPConfiguration> PC::getIPConfiguration() {
	IPConfiguration ipConfiguration = {this->ip, this->mask, this->gateway};
	if (ipConfiguration.isConfigurable())
		return {ipConfiguration};
	return {};
}

void PC::start() {

}

PC::~PC() {
	delete this->ip;
	delete this->mask;
	delete this->gateway;
	delete this->mac;
	delete this->linkAddress;
	delete this->physicalAddress;
}

