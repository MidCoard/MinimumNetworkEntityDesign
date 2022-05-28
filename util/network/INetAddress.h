//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_INETADDRESS_H
#define NETWORKDESIGN_INETADDRESS_H

class Socket;

#include "string"
#include "IP.h"
#include <utility>

class INetAddress {
public:
	INetAddress(IP ip, int port);

	IP getIp() const;

	int getPort() const;

	Socket createSocket() const;

private:
	IP ip;
	int port;
};

INetAddress createINetAddress(const std::string &ip);

INetAddress generatePhysicalAddress(int entityId, int id);

INetAddress generateLinkAddress(int entityId, int id);

#endif //NETWORKDESIGN_INETADDRESS_H
