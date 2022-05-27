//
// Created by 周蜀杰 on 2022/4/18.
//

#include "Socket.h"
#include "PhysicalLayer.h"

Socket::Socket(int port) :port(port) {
	this->internal = socket(AF_INET, SOCK_STREAM, 0);

}

void Socket::listen(PhysicalLayer *physicalLayer) {
	this->thread = new std::thread();


}

void Socket::send(INetAddress *address, const char *data, int len) const {
//todo
}
