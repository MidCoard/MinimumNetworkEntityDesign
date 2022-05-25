#include "NetworkLayer.h"

#include <utility>

NetworkLayer::NetworkLayer(IP ip, IP gateway) : ip(std::move(ip)), gateway(std::move(gateway)) {
}

NetworkLayer::NetworkLayer() : NetworkLayer(NULL_IP, NULL_IP) {
}

std::string NetworkLayer::getName() {
	return "NET";
}
