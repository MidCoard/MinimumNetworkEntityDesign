#include "NetworkEntity.h"

NetworkEntity::NetworkEntity(Layer *layer) :layer(layer) {}

NetworkEntity::~NetworkEntity() {
	delete layer;
}