//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_INETADDRESS_H
#define NETWORKDESIGN_INETADDRESS_H
#include "string"
#include "Socket.h"

class INetAddress {
public:
	INetAddress(std::string ip, int port);
	std::string getIp() const;
	int getPort() const;
	Socket createSocket() const;
private:
	const std::string ip;
	const int port;
};


#endif //NETWORKDESIGN_INETADDRESS_H
