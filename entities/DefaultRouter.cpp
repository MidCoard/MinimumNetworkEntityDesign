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
	auto* networkLayer = (RouterNetworkLayer *) this->layer;
	std::vector<bool> visited(this->network->getNodes().size(), false);
	visited.at(this->node) = true;
	for (int i = this->network->getHeads()[node]; i != -1; i = this->network->getLinks()[i]->next) {
		int port = this->network->getLinks()[i]->weight.first;
		if (port == kWanPort)
			continue;
		int subNode = this->network->getLinks()[i]->node;
		auto child = this->network->getNodes()[subNode];
		std::vector<IPConfiguration> configurations;
		IPConfiguration ipConfiguration = networkLayer->getIPConfiguration(port);
		if (ipConfiguration.isConfigurable())
			configurations.push_back(ipConfiguration);
		if (child->isIPAvailable()) {
			visited.at(subNode) = true;
			auto configuration = child->getIPConfiguration();
			configurations.insert(configurations.end(), configuration.begin(), configuration.end());
		} else
			dfsAllocateIP(subNode, &visited, &configurations);
		IP defaultIP = IP(255, 255, 255, 255);
		IP defaultMask = IP(255, 255, 255, 255);
		IP * defaultGateway = ipConfiguration.getGateway();
		for (auto &configuration: configurations) {
			defaultMask = defaultMask & *configuration.getMask() & defaultIP.mix(*configuration.getSegment() & *configuration.getMask());
			defaultIP = defaultIP & (*configuration.getSegment() & *configuration.getMask());
			if (configuration.getGateway() != nullptr && defaultGateway == nullptr)
				defaultGateway = configuration.getGateway();
		}
		delete ipConfiguration.getSegment();
		delete ipConfiguration.getMask();
		if (defaultGateway == nullptr || defaultGateway == ipConfiguration.getGateway())
			networkLayer->setIPConfiguration(port, new IP(defaultIP), new IP(defaultMask), defaultGateway);
		else
			networkLayer->setIPConfiguration(port, new IP(defaultIP), new IP(defaultMask), new IP(*defaultGateway));
	}
	std::vector<IPConfiguration> configurations;
	for (int i = 0; i<networkLayer->lowerLayers.size(); i++)
		if (networkLayer->getIPConfiguration(i).isConfigurable())
			configurations.push_back(networkLayer->getIPConfiguration(i));
	IP defaultIP = IP(255, 255, 255, 255);
	IP defaultMask = IP(255, 255, 255, 255);
	for (auto &configuration: configurations) {
		defaultMask = defaultMask & *configuration.getMask() & defaultIP.mix(*configuration.getSegment() & *configuration.getMask());
		defaultIP = defaultIP & (*configuration.getSegment() & *configuration.getMask());
	}
	IPConfiguration ipConfiguration = networkLayer->getIPConfiguration(kWanPort);
	delete ipConfiguration.getSegment();
	delete ipConfiguration.getMask();
	networkLayer->setIPConfiguration(kWanPort, new IP(defaultIP), new IP(defaultMask), ipConfiguration.getGateway());
	generatedIP = true;
}

void DefaultRouter::dfsAllocateIP(int node, std::vector<bool> *visited, std::vector<IPConfiguration> *configurations) {
	visited->at(node) = true;
	for (int i = this->network->getHeads()[node]; i != -1; i = this->network->getLinks()[i]->next) {
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

DefaultRouter::DefaultRouter(Network *network, int node, std::map<int, RouterConfiguration *> routerConfigurations)
		: Router(network, node, std::move(routerConfigurations)) {}

std::vector<IPConfiguration> DefaultRouter::getIPConfiguration() {
	generateIP();
	IPConfiguration ipConfiguration = ((RouterNetworkLayer *) this->layer)->getIPConfiguration(0);
	if (ipConfiguration.isConfigurable())
		return {ipConfiguration};
	return {};
}

std::string DefaultRouter::getName() {
	return "DefaultRouter";
}
