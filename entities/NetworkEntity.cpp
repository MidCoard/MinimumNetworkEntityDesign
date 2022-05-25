#include "NetworkEntity.h"

NetworkEntity::NetworkEntity(int node, Layer *layer) : node(node), layer(layer) {}

NetworkEntity::~NetworkEntity() {
	delete layer;
}