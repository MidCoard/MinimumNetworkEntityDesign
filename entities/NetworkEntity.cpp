#include "NetworkEntity.h"

const Executor kExecutor(10);

// the first Network pointer(avoid circular dependencies)
// node is to get its position in building part
// layer must be pointer because it is abstract
NetworkEntity::NetworkEntity(Network* network, int node, Layer *layer) : network(network), node(node), layer(layer) {}

NetworkEntity::~NetworkEntity() {
	delete this->layer;
}

bool NetworkEntity::isRouter() {
	return false;
}

bool NetworkEntity::isIPAvailable() {
	return false;
}

std::vector<IPConfiguration> NetworkEntity::getIPConfiguration() {
	return {};
}
