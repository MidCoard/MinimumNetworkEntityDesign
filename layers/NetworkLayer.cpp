#include "NetworkLayer.h"

NetworkLayer::NetworkLayer(IP *ip, IP *gateway) {
	this->ip = ip;
	this->gateway = gateway;
}

NetworkLayer::NetworkLayer() = default;

std::string NetworkLayer::getName() {
	return "NET";
}
