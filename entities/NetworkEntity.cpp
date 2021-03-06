#include "NetworkEntity.h"

Executor kExecutor(10);

// the first Network pointer(avoid circular dependencies)
// node is to get its position in building part
// layer must be pointer because it is abstract
NetworkEntity::NetworkEntity(Network *network, int node, Layer *layer) : network(network), node(node), layer(layer) {}

NetworkEntity::~NetworkEntity() {
	delete this->layer;
}

bool NetworkEntity::isIPAvailable() {
	return false;
}

std::vector<IPConfiguration> NetworkEntity::getIPConfiguration() {
	return {};
}

void NetworkEntity::start() {
	this->layer->start();
}

void NetworkEntity::stop() {
	this->layer->stop();
}

int NetworkEntity::getNode() const {
	return this->node;
}

void NetworkEntity::generateIP() {}

bool NetworkEntity::isISP() {
	return false;
}

bool NetworkEntity::isRouter() {
	return false;
}

bool NetworkEntity::isSwitch() {
	return false;
}
