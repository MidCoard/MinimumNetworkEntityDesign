#include "NetworkLayer.h"

#include <utility>

NetworkLayer::NetworkLayer(IP *ip, IP *gateway) : NetworkLayer(-1, ip, gateway) {}

std::string NetworkLayer::getRawName() {
	return "NET";
}

NetworkLayer::NetworkLayer(int id, IP *ip, IP *gateway) : Layer(id), ip(ip), gateway(gateway) {}
