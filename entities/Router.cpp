#include "Router.h"

Router::Router(int node, IP *segment, IP *mask) : NetworkEntity(node, new RouterNetworkLayer(segment, mask)) {
	this->segment = segment;
	this->mask = mask;
}

std::vector<std::string> Router::createLayers(int node, std::vector<int> ids) {
	for (int id: ids) {
		auto *linkLayer = new LinkLayer(new MAC(generateMAC()));
		auto *physicalLayer = new PhysicalLayer(new INetAddress(generatePhysicalAddress(node, id)));
		linkLayer->addLowerLayer(physicalLayer);
		this->layer->addLowerLayer(linkLayer);
	}
	return this->layer->generateGraph(node);
}

RouterNetworkLayer::RouterNetworkLayer(IP *segment, IP *mask) : NetworkLayer(segment, mask) {
}
