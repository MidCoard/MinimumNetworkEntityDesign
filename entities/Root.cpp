#include "Root.h"

const MAC *kRootMAC = new MAC("ee:ee:ee:ee:ee:ee");
const IP *kRootIP = new IP("0.0.0.0");
const IP *kRootMask = new IP("0.0.0.0");

Root::Root() : NetworkEntity(0, new RootAppLayer()) {
}

std::vector<std::string> Root::createLayers(int node, std::vector<int> ids) {
	auto *networkLayer = new RouterNetworkLayer((IP *) kRootIP, (IP *) kRootMask);
	this->layer->addLowerLayer(networkLayer);
	for (int id: ids) {
		auto *linkLayer = new LinkLayer(id,new MAC(generateMAC()));
		auto *physicalLayer = new PhysicalLayer(id,new INetAddress(generatePhysicalAddress(node, id)));
		linkLayer->addLowerLayer(physicalLayer);
		networkLayer->addLowerLayer(linkLayer);
	}
	return this->layer->generateGraph(node);
}
