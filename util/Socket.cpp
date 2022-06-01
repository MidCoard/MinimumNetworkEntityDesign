//
// Created by 周蜀杰 on 2022/4/18.
//

#include "Socket.h"
#include "PhysicalLayer.h"

Socket::Socket(int port) : port(port) {
	this->temp = new unsigned char[1024];
	this->internal = socket(AF_INET, SOCK_STREAM, 0);
	if (this->internal == -1)
		throw std::runtime_error("create socket failed");
	int option = 1;
	if (setsockopt(this->internal, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)))
		throw std::runtime_error("set socket option failed");
	option = 1048576;
	if (setsockopt(this->internal, SOL_SOCKET, SO_RCVBUF, &option, sizeof(option)))
		throw std::runtime_error("set socket size option failed");
	struct sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->internal, (struct sockaddr *) &addr, sizeof(addr)))
		throw std::runtime_error("bind socket failed");
	if (::listen(this->internal, 1000))
		throw std::runtime_error("listen socket failed");
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
		while ((len = recv(client, temp, sizeof(temp), 0)) != 0)
			block->write(temp, len);
		block->flip();
		shutdown(client, SHUT_RD);
		if (block->size() >= 13)
			physicalLayer->log("[SOCKET]from source " + block->viewMAC(0).str() + " to " + block->viewMAC(6).str() + " use Protocol " + std::to_string(block->view(12)));
		if (block->size() >= 22 && block->view(12) == 0)
			physicalLayer->log("[SOCKET]from source " + block->viewIP(13).str() + " to " + block->viewIP(17).str() + " use Protocol " + std::to_string(block->view(21)));
		if (block->size() >= 23 && block->view(21) == 0)
			physicalLayer->log("[SOCKET]app layer protocol " + std::to_string(block->view(22)));
		if (block->size() < 23 || block->view(22) != 0x60 || physicalLayer->getID() != 3)
			physicalLayer->receive(physicalLayer->getID(), block);
	}
}

void Socket::listen(PhysicalLayer *physicalLayer) {
	this->thread = new std::thread(&Socket::run, this, physicalLayer);
}

void Socket::send(const INetAddress &address, Block *block) {
	int client = socket(AF_INET, SOCK_STREAM, 0);
	if (client == -1)
		throw std::runtime_error("create socket failed");
	struct sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(address.getPort());
	addr.sin_addr.s_addr = htonl(address.getIp().intValue());
	if (connect(client, (struct sockaddr *) &addr, sizeof(addr))) {
		std::cerr << "connect to " << address.str() << " failed" << std::endl;
		return;
	}
	if (block->size() >= 13)
		printf("%s", ("[SOCKET]from source " + block->viewMAC(0).str() + " to " + block->viewMAC(6).str() + " use Protocol " + std::to_string(block->view(12))).c_str());
	if (block->size() >= 22 && block->view(12) == 0)
		printf("%s", ("[SOCKET]from source " + block->viewIP(13).str() + " to " + block->viewIP(17).str() + " use Protocol " + std::to_string(block->view(21))).c_str());
	if (block->size() >= 23 && block->view(21) == 0)
		printf("%s", ("[SOCKET]app layer protocol " + std::to_string(block->view(22))).c_str());
	while (block->getRemaining() > 0) {
		int len = block->read(temp, sizeof(temp));
		if (::send(client, temp, len, 0) == -1)
			throw std::runtime_error("send socket failed");
	}
	shutdown(client, SHUT_WR);
}

void Socket::close() {
	if (this->thread != nullptr) {
		shutdown(this->internal, SHUT_RDWR);
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
