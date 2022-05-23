//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_INETADDRESS_H
#define NETWORKDESIGN_INETADDRESS_H
#include "string"
#include "Socket.h"
#include "IP.h"

// new class will not be deleted in the whole process

class INetAddress {
public:
	INetAddress(IP ip, int port);
	const IP getIp() const;
	int getPort() const;
	Socket createSocket() const;
private:
	const IP ip;
	const int port;
};

INetAddress createINetAddress(const std::string& ip) {
	auto vector = util::split(ip, ":");
	if (vector.size() != 2)
		throw std::invalid_argument("invalid IP address and port");
	return {IP(vector[0]), std::stoi(vector[1])};
}


#endif //NETWORKDESIGN_INETADDRESS_H
