#include "Router.h"

Router::Router(Network* network, int node,std::map<int,RouterConfiguration> routerConfigurations) : NetworkEntity(network,node, new RouterNetworkLayer()), routerConfigurations(std::move(routerConfigurations)) {
}

std::vector<std::string> Router::createLayers(int node, std::vector<int> ids) {
	for (int id: ids) {
		if (!this->routerConfigurations.empty()) {
			RouterConfiguration routerConfiguration = routerConfigurations.at(id);
			((RouterNetworkLayer *) this->layer)->setIPConfiguration(id, routerConfiguration.getSegment(),
			                                                         routerConfiguration.getMask(),
			                                                         routerConfiguration.getGateway());
			auto *linkLayer = new LinkLayer(id);
			if (routerConfiguration.getMAC() != nullptr)
				linkLayer->setMAC(id, routerConfiguration.getMAC());
			else
				linkLayer->setMAC(id, new MAC(generateMAC()));
			this->layer->addLowerLayer(linkLayer);
			auto* physicalLayer = new PhysicalLayer(id, routerConfiguration.getLinkAddress() == nullptr ? new INetAddress(generateLinkAddress(node, id)) : routerConfiguration.getLinkAddress()
															 , routerConfiguration.getPhysicalAddress() == nullptr ? new INetAddress(generatePhysicalAddress(node ,id)) : routerConfiguration.getPhysicalAddress());
			linkLayer->addLowerLayer(physicalLayer);
		} else {
			((RouterNetworkLayer *) this->layer)->setIPConfiguration(id, nullptr, nullptr, nullptr);
			auto *linkLayer = new LinkLayer(id);
			linkLayer->setMAC(id, new MAC(generateMAC()));
			this->layer->addLowerLayer(linkLayer);
			auto *physicalLayer = new PhysicalLayer(id, new INetAddress(generateLinkAddress(node ,id)), new INetAddress(generatePhysicalAddress(node, id)));
			linkLayer->addLowerLayer(physicalLayer);
		}
	}
	return this->layer->generateGraph(node);
}

bool Router::isRouter() {
	return true;
}

bool Router::isRouterMaster() {
	return false;
}

bool Router::isIPAvailable() {
	return true;
}
RouterConfiguration::RouterConfiguration(IP *segment, IP *mask, IP *gateway, MAC *mac, INetAddress *linkAddress,
                                         INetAddress *physicalAddress)
		: segment(segment), mask(mask), gateway(gateway), mac(mac),linkAddress(linkAddress), physicalAddress(physicalAddress) {}

IP *RouterConfiguration::getSegment() {
	return this->segment;
}

IP * RouterConfiguration::getMask() {
	return this->mask;
}

IP * RouterConfiguration::getGateway() {
	return this->gateway;
}

MAC* RouterConfiguration::getMAC() {
	return this->mac;
}

INetAddress* RouterConfiguration::getPhysicalAddress() {
	return this->physicalAddress;
}

INetAddress *RouterConfiguration::getLinkAddress() {
	return this->linkAddress;
}

RouterNetworkLayer::RouterNetworkLayer() : NetworkLayer() {}

RouterNetworkLayer::RouterNetworkLayer(int id) : NetworkLayer(id) {}

IPConfiguration RouterNetworkLayer::getIPConfiguration(int id) {
	return this->configurations.at(id);
}

unsigned long RouterNetworkLayer::size() {
	return this->configurations.size();
}

