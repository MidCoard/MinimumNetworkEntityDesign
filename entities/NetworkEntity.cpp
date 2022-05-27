#include "NetworkEntity.h"

NetworkEntity::NetworkEntity(Network* network, int node, Layer *layer) : network(network), node(node), layer(layer) {}

NetworkEntity::~NetworkEntity() {
	delete layer;
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
