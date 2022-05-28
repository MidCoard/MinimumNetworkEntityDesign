//
// Created by 周蜀杰 on 2022/5/9.
//

#include "PhysicalLayer.h"

#include <utility>

PhysicalLayer::PhysicalLayer(NetworkEntity * networkEntity, INetAddress linkAddress,INetAddress physicalAddress) : PhysicalLayer(-1,networkEntity,std::move(linkAddress),std::move(physicalAddress)) {}

std::string PhysicalLayer::getRawName() {
	return "PHY";
}

PhysicalLayer::PhysicalLayer(int id, NetworkEntity * networkEntity, INetAddress linkAddress, INetAddress physicalAddress) : Layer(id, networkEntity), linkAddress(std::move(linkAddress)), physicalAddress(std::move(physicalAddress)){}

void PhysicalLayer::start() {
	Layer::start();
	this->socket = new Socket(this->linkAddress.createSocket());
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

void PhysicalLayer::dealReceive(int id, Block* block) {
	if (this->upperLayers.size() == 1)
		this->upperLayers[0]->receive(id, new Block(block));
	else
		throw std::invalid_argument("physical layer must have one upper layer");
}

void PhysicalLayer::dealSend(Block* block) {
	if (this->socket != nullptr)
		this->socket->send(this->physicalAddress, block);
}
