#include "NetworkLayer.h"

#include <utility>

NetworkLayer::NetworkLayer() : NetworkLayer(-1) {}

std::string NetworkLayer::getRawName() {
	return "NET";
}

NetworkLayer::NetworkLayer(int id) : Layer(id) {}

void NetworkLayer::setIP(int id, IP *ip, IP *mask, IP *gateway) {
	ips.insert({id, ip});
	masks.insert({id, mask});
	gateways.insert({id, gateway});
}
