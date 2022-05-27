#include "INetAddress.h"

INetAddress::INetAddress(IP ip, int port) : port(port), ip(std::move(ip)) {}

IP INetAddress::getIp() const {
	return this->ip;
}

int INetAddress::getPort() const {
	return port;
}

Socket INetAddress::createSocket() const {
	return Socket(this->port);
}

INetAddress createINetAddress(const std::string &ip) {
	auto vector = util::split(ip, ":");
	if (vector.size() != 2)
		throw std::invalid_argument("invalid IP address and port " + ip);
	return {IP(vector[0]), std::stoi(vector[1])};
}

INetAddress generatePhysicalAddress(int entityId, int id) {
	//because our entityId starts from 0, we need to add 1 to the entityId
	entityId++;
	return {localhost, 10000 + entityId * 1000 + 100 + id};
}

INetAddress generateLinkAddress(int entityId, int id) {
	//because our entityId starts from 0, we need to add 1 to the entityId
	entityId++;
	return {localhost, 10000 + entityId * 1000 + 200 + id};
}