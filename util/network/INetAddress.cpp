//
// Created by 周蜀杰 on 2022/4/18.
//

#include "INetAddress.h"

#include <utility>

INetAddress::INetAddress(IP * ip, int port) : port(port), ip(ip) {}

const IP * INetAddress::getIp() const {
	return this->ip;
}

int INetAddress::getPort() const {
	return port;
}

Socket INetAddress::createSocket() const {
	//todo create socket
}
