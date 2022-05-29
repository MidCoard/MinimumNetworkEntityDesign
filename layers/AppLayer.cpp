//
// Created by 周蜀杰 on 2022/4/18.
//

#include "AppLayer.h"

std::string AppLayer::getRawName() {
	return "APP";
}

AppLayer::AppLayer(NetworkEntity *networkEntity) : Layer(0, networkEntity) {
}

void AppLayer::handleSend(Block *block) {
	if (this->lowerLayers.size() == 1)
		this->lowerLayers[0]->send(new Block(block));
	else
		throw std::invalid_argument(" app layer must have one lower layer");
}

void AppLayer::handleReceive(int id, Block *block) {
	std::vector<unsigned char> data = block->read();
	std::string str(data.begin(), data.end());
	this->log("receive data: " + str);
}
