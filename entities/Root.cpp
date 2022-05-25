#include "Root.h"

Root::Root() : NetworkEntity(0, new RootAppLayer()) {
}

std::vector<std::string> Root::createLayers(int node, std::vector<int> ids) {
	auto *networkLayer = new RouterNetworkLayer((IP *) kRootIP, (IP *) kRootMask);
	this->layer->addLowerLayer(networkLayer);
	for (int id: ids) {
		auto *linkLayer = new LinkLayer(new MAC(generateMAC()));
		auto *physicalLayer = new PhysicalLayer(new INetAddress(generatePhysicalAddress(node, id)));
		linkLayer->addLowerLayer(physicalLayer);
		networkLayer->addLowerLayer(linkLayer);
	}
}
