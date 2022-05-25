#include "PC.h"

PC::PC(int node, IP *ip, IP *gateway, MAC *mac, INetAddress *physicalAddress) : NetworkEntity(node, new AppLayer()) {
	this->ip = ip;
	this->gateway = gateway;
	this->mac = mac;
	this->physicalAddress = physicalAddress;
}

PC::~PC() {
	delete ip;
	delete gateway;
	delete mac;
	delete physicalAddress;
}

std::vector<std::string> PC::createLayers(int node, std::vector<int> ids) {
	if (ids.size() != 1)
		throw std::invalid_argument("PC port size != 1");
	auto *networkLayer = new NetworkLayer(ids[0], ip, gateway);
	this->layer->addLowerLayer(networkLayer);
	if (mac == nullptr)
		mac = new MAC(generateMAC());
	auto *linkLayer = new LinkLayer(ids[0], mac);
	networkLayer->addLowerLayer(linkLayer);
	if (physicalAddress == nullptr)
		physicalAddress = new INetAddress(generatePhysicalAddress(node, ids[0]));
	auto *physicalLayer = new PhysicalLayer(ids[0], physicalAddress);
	linkLayer->addLowerLayer(physicalLayer);
	return this->layer->generateGraph(node);
}

