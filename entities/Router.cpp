#include "Router.h"

Router::Router(IP* segment, IP* mask) : NetworkEntity(new RouterNetworkLayer(segment,mask)) {
	this->segment = segment;
	this->mask = mask;
}

void Router::createLayers(int node, std::vector<int> ids) {
	for (int id : ids) {
		auto * linkLayer = new LinkLayer(new MAC(generateMAC()));
		auto * physicalLayer = new PhysicalLayer(new INetAddress(generatePhysicalAddress(node, id)));
		linkLayer->addLowerLayer(physicalLayer);
		this->layer->addLowerLayer(linkLayer);
	}
}

RouterNetworkLayer::RouterNetworkLayer(IP *segment, IP *mask) : NetworkLayer(segment, mask) {
}
