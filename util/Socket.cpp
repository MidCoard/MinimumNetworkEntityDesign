//
// Created by 周蜀杰 on 2022/4/18.
//

#include "Socket.h"
#include "PhysicalLayer.h"
#include "unistd.h"

Socket::Socket(int port) : port(port) {
	this->temp = new unsigned char[1024];
	return;
	this->internal = socket(AF_INET, SOCK_DGRAM, 0);
	if (this->internal == -1) {
		std::cerr << port << std::endl;
		throw std::runtime_error("create socket failed");
	}
	int option = 1;
	if (setsockopt(this->internal, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&option), sizeof(option)))
		throw std::runtime_error("set socket option failed");
	option = 1048576;
	if (setsockopt(this->internal, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char *>(&option), sizeof(option)))
		throw std::runtime_error("set socket size option failed");
	struct sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->internal, (struct sockaddr *) &addr, sizeof(addr)))
		throw std::runtime_error("bind socket failed");
//	if (::listen(this->internal, 1000))
//		throw std::runtime_error("listen socket failed");
}

void Socket::run(PhysicalLayer *physicalLayer) const {
	while (true) {
		struct sockaddr_in addr{};
		socklen_t addrlen = sizeof(addr);
		int client = accept(this->internal, (struct sockaddr *) &addr, &addrlen);
		if (shouldStop)
			break;
		if (client == -1)
			continue;
		auto *block = new Block();
		int len;
		while ((len = recv(client, reinterpret_cast<char *>(temp), sizeof(temp), 0)) != 0)
			block->write(temp, len);
		block->flip();
#ifdef WINDOWS
		shutdown(client, SD_BOTH);
#else
		shutdown(client, SHUT_RDWR);
#endif
		::close(client);
		physicalLayer->receive(physicalLayer->getID(), block);
	}
}

void Socket::listen(PhysicalLayer *physicalLayer) {
	this->thread = new std::thread(&Socket::run, this, physicalLayer);
}

void Socket::send(const INetAddress &address, Block *block) {
	int client = socket(AF_INET, SOCK_DGRAM, 0);
	if (client == -1) {
		std::cerr << "create socket failed" << std::endl;
		return;
	}
	struct sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(address.getPort());
	addr.sin_addr.s_addr = htonl(address.getIp().intValue());
	if (connect(client, (struct sockaddr *) &addr, sizeof(addr))) {
#ifdef WINDOWS
		shutdown(client, SD_BOTH);
#else
		shutdown(client, SHUT_RDWR);
#endif
		::close(client);
		std::cerr << "connect to " << address.str() << " failed" << std::endl;
		return;
	}
	while (block->getRemaining() > 0) {
		int len = block->read(temp, sizeof(temp));
		if (::send(client, reinterpret_cast<const char *>(temp), len, 0) == -1) {
			std::cerr << "send socket failed" << std::endl;
			break;
		}
	}
#ifdef WINDOWS
	shutdown(client, SD_BOTH);
#else
	shutdown(client, SHUT_RDWR);
#endif
	::close(client);
}

void Socket::close() {
	if (this->thread != nullptr) {
#ifdef WINDOWS
		shutdown(this->internal, SD_BOTH);
#else
		shutdown(this->internal, SHUT_RDWR);
#endif
		this->shouldStop = true;
		auto *block = new Block();
		send(INetAddress(LOCAL0, this->port), block);
		delete block;
		this->thread->join();
		delete this->thread;
		this->thread = nullptr;
	}
}

Socket::~Socket() {
	delete[] this->temp;
}
