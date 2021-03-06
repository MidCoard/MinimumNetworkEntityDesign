#include "PC.h"

#include <utility>
#include "DHCPHelper.h"

// why here using pointer to IP, MAC etc.
// to present null IP, MAC etc.
// all should be deleted in layers not in the entity
PC::PC(Network *network, int node, IP *ip, IP *mask, IP *gateway, MAC *mac, INetAddress *linkAddress,
       INetAddress *physicalAddress)
		: NetworkEntity(network, node, new AppLayer(this)), ip(ip), mask(mask), gateway(gateway), mac(mac),
		  linkAddress(linkAddress), physicalAddress(physicalAddress) {}


std::vector<std::string> PC::createLayers(int node, std::vector<int> ids) {
	if (ids.size() != 1)
		throw std::invalid_argument("PC network interface size should be 1");
	this->networkLayer = new NetworkLayer(ids[0], this);
	networkLayer->setIPConfiguration(ids[0], this->ip,
	                                 this->mask,
	                                 this->gateway);
	if (networkLayer->getIPConfiguration(0).isValid()) {
		networkLayer->isIPValid = true;
		networkLayer->routeTable.updateShort(LOCAL0, LOCAL0, 10, *this->gateway, 0);
	}
	this->layer->addLowerLayer(networkLayer);
	auto *linkLayer = new LinkLayer(ids[0], this);
	if (mac == nullptr)
		mac = new MAC(generateMAC());
    std::cout<<this->node<<" MAC address:"<< mac->str()<<std::endl;
	linkLayer->setMAC(ids[0], *mac);
	networkLayer->addLowerLayer(linkLayer);
	if (linkAddress == nullptr)
		linkAddress = new INetAddress(generateLinkAddress(node, ids[0]));
	if (physicalAddress == nullptr)
		physicalAddress = new INetAddress(generatePhysicalAddress(node, ids[0]));
	auto *physicalLayer = new PhysicalLayer(ids[0], this, *linkAddress, *physicalAddress);
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

PC::~PC() {
	delete this->ip;
	delete this->mask;
	delete this->gateway;
	delete this->mac;
	delete this->linkAddress;
	delete this->physicalAddress;
}

void PC::start() {
	NetworkEntity::start();
	if (!this->networkLayer->isIPValid) {
		this->networkLayer->sendDHCP();
		dhcp::request(this->networkLayer);
	}
}

std::string PC::getName() {
	return "PC";
}

void PC::ping(IP ip) {
	this->networkLayer->sendICMP(std::move(ip));
}

void PC::releaseIP() {
	this->networkLayer->sendDHCPRelease();
}

void PC::applyIP() {
	this->networkLayer->sendDHCP();
}

void PC::send(const IP &ip, unsigned char *buffer, long long int size) {
	auto appLayer = (AppLayer *) this->layer;
	appLayer->sendUDPData(ip, buffer, size);
}
