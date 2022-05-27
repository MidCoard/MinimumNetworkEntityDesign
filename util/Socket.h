//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_SOCKET_H
#define NETWORKDESIGN_SOCKET_H

class PhysicalLayer;

#include <sys/socket.h>
#include <netinet/in.h>
#include "thread"
#include "network/INetAddress.h"
#include "Block.h"

class Socket {
public:
	explicit Socket(int port);

	static void send(const INetAddress& address, Block * block) ;

	void listen(PhysicalLayer* physicalLayer);

	void close();

private:
	int internal;

	std::thread * thread = nullptr;

	void run(PhysicalLayer *physicalLayer) const;

};


#endif //NETWORKDESIGN_SOCKET_H
