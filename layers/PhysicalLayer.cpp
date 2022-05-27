//
// Created by 周蜀杰 on 2022/5/9.
//

#include "PhysicalLayer.h"

#include <utility>

PhysicalLayer::PhysicalLayer(INetAddress *linkAddress,INetAddress * physicalAddress) : PhysicalLayer(-1,linkAddress,physicalAddress) {}

std::string PhysicalLayer::getRawName() {
	return "PHY";
}

PhysicalLayer::PhysicalLayer(int id,INetAddress* linkAddress, INetAddress * physicalAddress) : Layer(id), linkAddress(linkAddress), physicalAddress(physicalAddress), socket(nullptr), sendThread(nullptr){}

void PhysicalLayer::start() {
	Layer::start();
	this->socket = new Socket(this->linkAddress->createSocket());
	socket->listen(this);
}

void PhysicalLayer::stop() {
	Layer::stop();
	if (this->socket != nullptr) {
		this->socket->close();
		delete this->socket;
		this->socket = nullptr;
	}
}

void PhysicalLayer::deal(Block *pBlock) {

}
