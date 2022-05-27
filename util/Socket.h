//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_SOCKET_H
#define NETWORKDESIGN_SOCKET_H

class PhysicalLayer;

#include <sys/socket.h>
#include "thread"
#include "network/INetAddress.h"

class Socket {
public:
	explicit Socket(int port);
	void send(INetAddress* address, const char *data, int len) const;
	//todo
	void listen(PhysicalLayer* physicalLayer);

private:
	int port;
	int internal;
	std::thread * thread;
};


#endif //NETWORKDESIGN_SOCKET_H
