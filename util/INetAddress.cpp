//
// Created by 周蜀杰 on 2022/4/18.
//

#include "INetAddress.h"

#include <utility>

INetAddress::INetAddress(std::string ip, int port) : port(port), ip(std::move(ip)) {}

std::string INetAddress::getIp() const {
	return ip;
}

int INetAddress::getPort() const {
	return port;
}

Socket INetAddress::createSocket() const {
	//todo create socket
}
