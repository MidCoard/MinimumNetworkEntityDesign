#include "ISP.h"

const IP *kRootIP = new IP("0.0.0.0");
const IP *kRootMask = new IP("0.0.0.0");

ISP::ISP(Network* network) : NetworkEntity(network, 0, new RootAppLayer()) {
}

std::vector<std::string> ISP::createLayers(int node, std::vector<int> ids) {
	auto *networkLayer = new RouterNetworkLayer();
	this->layer->addLowerLayer(networkLayer);
	for (int id: ids) {
		networkLayer->setIP(id, nullptr, nullptr, nullptr);
		auto *linkLayer = new LinkLayer(id);
		linkLayer->setMAC(id, new MAC(generateMAC()));
		networkLayer->addLowerLayer(linkLayer);
		auto *physicalLayer = new PhysicalLayer(id,new INetAddress(generatePhysicalAddress(node, id)));
		linkLayer->addLowerLayer(physicalLayer);
	}
	return this->layer->generateGraph(node);
}

bool ISP::isRouterMaster() {
	return false;
}

bool ISP::isIPAvailable() {
	return true;
}

IP *ISP::getIP() {
	return const_cast<IP *>(kRootIP);
}

IP *ISP::allocateSegment(int node, IP segment, IP *mask) {
	bool contains = false;
	for (int i = this->network->getHeads()[this->node];i != -1;i = this->network->getLinks()[i]->next)
		if (this->network->getLinks()[i]->node == node) {
			contains = true;
			break;
		}
	if (!contains)
		return nullptr;
	if (mask != nullptr)
		if ((segment & *mask) != segment)
			return nullptr;
	return new IP(segment);
}
