//
// Created by 周蜀杰 on 2022/5/26.
//

#include "DefaultRouter.h"

#include <utility>
#include "Network.h"

const int kWanPort = 0;

void DefaultRouter::generateIP() {
	if (generatedIP)
		return;
	std::vector<bool> visited(this->network->getNodes().size(), false);
	std::vector<IPConfiguration> configurations;
	for (int i = 0; i <((RouterNetworkLayer *) this->layer)->size();i++ ) {
		IPConfiguration ipConfiguration = ((RouterNetworkLayer *) this->layer)->getIPConfiguration(i);
		if (ipConfiguration.isConfigurable())
			configurations.push_back(ipConfiguration);
	}
	dfsAllocateIP(this->node, &visited, &configurations);
	IP defaultIP = IP(255, 255, 255, 255);
	IP defaultMask = IP(255, 255, 255, 255);
	for (auto &configuration : configurations)
		if (configuration.getSegment() != nullptr && configuration.getMask() != nullptr) {
			defaultIP = defaultIP & (*configuration.getSegment() & *configuration.getMask());
			defaultMask = defaultMask & *configuration.getMask();
		}
	((RouterNetworkLayer *) this->layer)->setIPConfiguration(kWanPort, new IP(defaultIP),new IP(defaultMask), new IP(*((RouterNetworkLayer *) this->layer)->getIPConfiguration(kWanPort).getGateway()));
	generatedIP = true;
}

void DefaultRouter::dfsAllocateIP(int node, std::vector<bool> *visited, std::vector<IPConfiguration> *configurations) {
	visited->at(node) = true;
	for (int i = this->network->getHeads()[node]; i != -1; i = this->network->getLinks()[i]->next) {
		if (node == this->node && this->network->getLinks()[i]->weight.first == kWanPort)
			continue;
		int subNode = this->network->getLinks()[i]->node;
		if (visited->at(subNode))
			continue;
		auto child = this->network->getNodes()[subNode];
		if (child->isIPAvailable()) {
			visited->at(subNode) = true;
			auto configuration = child->getIPConfiguration();
			configurations->insert(configurations->end(), configuration.begin(), configuration.end());
		} else
			dfsAllocateIP(subNode, visited, configurations);
	}
}



DefaultRouter::DefaultRouter(Network *network, int node,std::map<int, RouterConfiguration> routerConfigurations) : Router(network, node, std::move(routerConfigurations)) {}

std::vector<IPConfiguration> DefaultRouter::getIPConfiguration() {
	generateIP();
	IPConfiguration ipConfiguration = ((RouterNetworkLayer *) this->layer)->getIPConfiguration(0);
	if (ipConfiguration.isConfigurable())
		return {ipConfiguration};
	return {};
}

void DefaultRouter::start() {

}
