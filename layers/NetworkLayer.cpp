#include "NetworkLayer.h"

#include <utility>

NetworkLayer::NetworkLayer() : NetworkLayer(-1) {}

std::string NetworkLayer::getRawName() {
	return "NET";
}

NetworkLayer::NetworkLayer(int id) : Layer(id) {}

void NetworkLayer::setIPConfiguration(int id, IP *segment, IP *mask, IP *gateway) {
	configurations.insert_or_assign(id, IPConfiguration(segment, mask, gateway));
}
