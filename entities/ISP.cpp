#include "ISP.h"
#include "Network.h"

const IP *kRootIP = new IP("0.0.0.0");
const IP *kRootMask = new IP("0.0.0.0");

ISP::ISP(Network* network) : NetworkEntity(network, 0, new RootAppLayer(this)) {
}

std::vector<std::string> ISP::createLayers(int node, std::vector<int> ids) {
	this->networkLayer = new RouterNetworkLayer(this);
	this->layer->addLowerLayer(networkLayer);
	auto* emptyLinkLayer = new EmptyLinkLayer(0, this);
	this->networkLayer->addLowerLayer(emptyLinkLayer);
	auto* emptyPhysicalLayer = new EmptyPhysicalLayer(0, this);
	emptyLinkLayer->addLowerLayer(emptyPhysicalLayer);
	for (int id: ids) {
		networkLayer->setIPConfiguration(id, nullptr, nullptr, nullptr);
		auto *linkLayer = new LinkLayer(id,this);
		linkLayer->setMAC(id, generateMAC());
		networkLayer->addLowerLayer(linkLayer);
		auto *physicalLayer = new PhysicalLayer(id,this,generateLinkAddress(node ,id),generatePhysicalAddress(node, id));
		linkLayer->addLowerLayer(physicalLayer);
	}
	return this->layer->generateGraph(node);
}

void ISP::start() {
	this->networkLayer->setIPConfiguration(0, const_cast<IP *>(kRootIP), const_cast<IP *>(kRootMask),
	                                       const_cast<IP *>(kRootIP));
	this->networkLayer->isIPValid = true;
	this->networkLayer->table = new DHCPTable(*kRootIP, *kRootMask);
	NetworkEntity::start();
}

std::string ISP::getName() {
	return "ISP";
}

RootAppLayer::RootAppLayer(NetworkEntity *networkEntity) : AppLayer(networkEntity) {
}

void EmptyLinkLayer::start() {}

void EmptyLinkLayer::stop() {}

EmptyLinkLayer::EmptyLinkLayer(int id, NetworkEntity *networkEntity) : LinkLayer(id, networkEntity) {}

void EmptyLinkLayer::handleReceive(int id, Block *block) {}

void EmptyLinkLayer::handleSend(Block *block) {}

void EmptyPhysicalLayer::start() {}

void EmptyPhysicalLayer::stop() {}

void EmptyPhysicalLayer::handleReceive(int id, Block *block) {}

void EmptyPhysicalLayer::handleSend(Block *block) {}

EmptyPhysicalLayer::EmptyPhysicalLayer(int id, NetworkEntity *networkEntity) : PhysicalLayer(id, networkEntity,
                                                                                             INetAddress(LOCALHOST, 0), INetAddress(LOCALHOST, 0)) {}
