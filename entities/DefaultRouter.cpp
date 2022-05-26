//
// Created by 周蜀杰 on 2022/5/26.
//

#include "DefaultRouter.h"

#include <utility>

void DefaultRouter::allocateIP() {
	std::vector<bool> visited(this->network->getNodes().size(), false);
	std::vector<IP *> ips;
	this->dsfAllocateIP(this->node, &visited, &ips);
	IP defaultIP = IP(255, 255, 255, 255);
	for (auto ip : ips)
		defaultIP = defaultIP & *ip;
	if (this->segment != nullptr) {
		if ((defaultIP & *this->segment) != *this->segment)
			throw std::invalid_argument("Subnet IP " + defaultIP.str() + " is not in the segment " + this->segment->str());
		this->segment = this->network->getRoot()->tryAllocateSegment();
	} else
		this->segment = this->network->getRoot()->allocateSegment(this->node, defaultIP, this->mask);
	if (this->segment == nullptr)
		throw std::invalid_argument("Cannot allocate segment");
	if (this->mask == nullptr)
		this->mask = new IP(this->segment->getMask());
}

void DefaultRouter::dsfAllocateIP(int node, std::vector<bool> *visited, std::vector<IP *> *ips) {
	visited->at(node) = true;
	for (int i = this->network->getHeads()[node]; i != -1; i = this->network->getLinks()[i]->next) {
		int subNode = this->network->getLinks()[i]->node;
		if (visited->at(subNode))
			continue;
		auto child = this->network->getNodes()[subNode];
		if (child->isRouterMaster() && child->isIPAvailable())
			ips->push_back(child->getIP());
	}
}

DefaultRouter::DefaultRouter(Network *network, int node, IP *ip, MAC *mac, INetAddress *physicalAddress,
                             std::map<int, RouterConfiguration> routerConfigurations) : Router(network, node, ip, mac,
																								 physicalAddress,routerConfigurations) {

}
