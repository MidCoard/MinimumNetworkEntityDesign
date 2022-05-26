#include "PC.h"

PC::PC(Network* network,int node, IP *ip, IP* mask, IP *gateway, MAC *mac, INetAddress *physicalAddress) : NetworkEntity(network, node, new AppLayer()), ip(ip), mask(mask), gateway(gateway), mac(mac), physicalAddress(physicalAddress) {}

PC::~PC() {
	delete ip;
	delete mask;
	delete gateway;
	delete mac;
	delete physicalAddress;
}

std::vector<std::string> PC::createLayers(int node, std::vector<int> ids) {
	if (ids.size() == 1)
		throw std::invalid_argument("PC network interface size should be 1");
	auto *networkLayer = new NetworkLayer(ids[0]);
	networkLayer->setIP(ids[0], this->ip, this->mask, this->gateway);
	this->layer->addLowerLayer(networkLayer);
	auto *linkLayer = new LinkLayer(ids[0]);
	if (mac == nullptr)
		mac = new MAC(generateMAC());
	linkLayer->setMAC(ids[0], mac);
	networkLayer->addLowerLayer(linkLayer);
	if (physicalAddress == nullptr)
		physicalAddress = new INetAddress(generatePhysicalAddress(node, ids[0]));
	auto *physicalLayer = new PhysicalLayer(ids[0], physicalAddress);
	linkLayer->addLowerLayer(physicalLayer);
	return this->layer->generateGraph(node);
}

bool PC::isIPAvailable() {
	return true;
}

IP *PC::getIP() {
	return this->ip;
}

