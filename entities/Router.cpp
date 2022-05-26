#include "Router.h"

Router::Router(Network* network, int node,IP* ip,MAC*mac,INetAddress* physicalAddress, std::map<int,RouterConfiguration> routerConfigurations) : NetworkEntity(network,node, new RouterNetworkLayer()), routerConfigurations(std::move(routerConfigurations)),ip(ip),mac(mac),physicalAddress(physicalAddress) {
}

std::vector<std::string> Router::createLayers(int node, std::vector<int> ids) {
	for (int id: ids) {
		if (!this->routerConfigurations.empty()) {
			RouterConfiguration routerConfiguration = routerConfigurations.at(id);
			((RouterNetworkLayer *) this->layer)->setIP(id, routerConfiguration.getSegment(), routerConfiguration.getMask(), routerConfiguration.getGateway());
			auto *linkLayer = new LinkLayer(id);
			if (routerConfiguration.getMAC() != nullptr)
				linkLayer->setMAC(id, routerConfiguration.getMAC());
			else
				linkLayer->setMAC(id, new MAC(generateMAC()));
			this->layer->addLowerLayer(linkLayer);
			PhysicalLayer* physicalLayer;
			if (routerConfiguration.getPhysicalAddress() != nullptr)
				physicalLayer = new PhysicalLayer(id, routerConfiguration.getPhysicalAddress());
			else
				physicalLayer = new PhysicalLayer(id, new INetAddress(generatePhysicalAddress(node ,id)));
			linkLayer->addLowerLayer(physicalLayer);
		} else {
			((RouterNetworkLayer *) this->layer)->setIP(id, nullptr, nullptr, nullptr);
			auto *linkLayer = new LinkLayer(id);
			linkLayer->setMAC(id, new MAC(generateMAC()));
			this->layer->addLowerLayer(linkLayer);
			auto *physicalLayer = new PhysicalLayer(id, new INetAddress(generatePhysicalAddress(node, id)));
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

void Router::setWAN(int port) {
	this->wan = port;
}

RouterConfiguration::RouterConfiguration(IP *segment, IP *mask, IP *gateway, MAC *mac, INetAddress *physicalAddress) : segment(segment), mask(mask), gateway(gateway), mac(mac), physicalAddress(physicalAddress) {}

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

RouterNetworkLayer::RouterNetworkLayer() : NetworkLayer() {}

RouterNetworkLayer::RouterNetworkLayer(int id) : NetworkLayer(id) {}

