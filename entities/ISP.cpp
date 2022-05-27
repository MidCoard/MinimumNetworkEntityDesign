#include "ISP.h"
#include "Network.h"

const IP *kRootIP = new IP("0.0.0.0");
const IP *kRootMask = new IP("0.0.0.0");

ISP::ISP(Network* network) : NetworkEntity(network, 0, new RootAppLayer()) {
}

std::vector<std::string> ISP::createLayers(int node, std::vector<int> ids) {
	auto *networkLayer = new RouterNetworkLayer();
	this->layer->addLowerLayer(networkLayer);
	for (int id: ids) {
		networkLayer->setIPConfiguration(id, nullptr, nullptr, nullptr);
		auto *linkLayer = new LinkLayer(id);
		linkLayer->setMAC(id, generateMAC());
		networkLayer->addLowerLayer(linkLayer);
		auto *physicalLayer = new PhysicalLayer(id,generateLinkAddress(node ,id),generatePhysicalAddress(node, id));
		linkLayer->addLowerLayer(physicalLayer);
	}
	return this->layer->generateGraph(node);
}

void ISP::start() {

}
