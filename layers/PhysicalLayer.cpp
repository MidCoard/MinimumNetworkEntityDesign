//
// Created by 周蜀杰 on 2022/5/9.
//

#include "PhysicalLayer.h"

#include <utility>

PhysicalLayer::PhysicalLayer(INetAddress *linkAddress,INetAddress * physicalAddress) : PhysicalLayer(-1,linkAddress,physicalAddress) {}

std::string PhysicalLayer::getRawName() {
	return "PHY";
}

PhysicalLayer::PhysicalLayer(int id,INetAddress* linkAddress, INetAddress * physicalAddress) : Layer(id),linkAddress(linkAddress), physicalAddress(physicalAddress) {
}

void PhysicalLayer::start() {
	this->socket = new Socket(this->linkAddress->createSocket());
	socket->listen(this);
}
