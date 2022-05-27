//
// Created by 周蜀杰 on 2022/4/18.
//

#include "Socket.h"
#include "PhysicalLayer.h"

Socket::Socket(int port) :port(port) {
	this->internal = socket(AF_INET, SOCK_STREAM, 0);
	if (this->internal == -1)
		throw std::runtime_error("create socket failed");
	int option = 1;
	if (setsockopt(this->internal, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)))
		throw std::runtime_error("set socket option failed");
	struct sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->internal, (struct sockaddr *) &addr, sizeof(addr)))
		throw std::runtime_error("bind socket failed");
	if (::listen(this->internal, 100))
		throw std::runtime_error("listen socket failed");
}

unsigned char kData[1024];

void Socket::run(PhysicalLayer *physicalLayer) const {
	while (true) {
		struct sockaddr_in addr{};
		socklen_t addrlen = sizeof(addr);
		int client = accept(this->internal, (struct sockaddr *) &addr, &addrlen);
		if (shouldStop)
			break;
		if (client == -1)
			continue;
		Block block;
		int len;
		while ((len = recv(client, kData, sizeof(kData), 0)) != 0)
			block.write(kData, len);
		block.flip();
		physicalLayer->receive(physicalLayer->getID(), block);
	}
}

void Socket::listen(PhysicalLayer *physicalLayer) {
	this->thread = new std::thread(&Socket::run, this, physicalLayer);
}

void Socket::send(const INetAddress& address, Block block) {
	int client = socket(AF_INET, SOCK_STREAM, 0);
	if (client == -1)
		throw std::runtime_error("create socket failed");
	struct sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(address.getPort());
	addr.sin_addr.s_addr = htonl(address.getIp().intValue());
	if (connect(client, (struct sockaddr *) &addr, sizeof(addr)))
		throw std::runtime_error("connect socket failed");
	while (block.getRemaining() > 0) {
		int len = block.read(kData, sizeof(kData));
		if (::send(client, kData, len, 0) == -1)
			throw std::runtime_error("send socket failed");
	}
	shutdown(client, SHUT_RDWR);
}

void Socket::close() {
	if (this->thread != nullptr) {
		shutdown(this->internal, SHUT_RDWR);
		this->shouldStop = true;
		send(INetAddress(local0,this->port), Block());
		this->thread->join();
		delete this->thread;
		this->thread = nullptr;
	}
}
