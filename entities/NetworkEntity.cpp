#include "NetworkEntity.h"

const Executor kExecutor(10);

NetworkEntity::NetworkEntity(Network* network, int node, Layer *layer) : network(network), node(node), layer(layer) {}

NetworkEntity::~NetworkEntity() {
	delete this->layer;
}

bool NetworkEntity::isRouter() {
	return false;
}

bool NetworkEntity::isRouterMaster() {
	return true;
}

bool NetworkEntity::isIPAvailable() {
	return false;
}

std::vector<IPConfiguration> NetworkEntity::getIPConfiguration() {
	return {};
}
