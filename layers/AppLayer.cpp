//
// Created by 周蜀杰 on 2022/4/18.
//

#include "AppLayer.h"

std::string AppLayer::getRawName() {
	return "APP";
}

AppLayer::AppLayer() : Layer(0) {
}

void AppLayer::dealSend(Block *block) {
	if (this->lowerLayers.size() == 1)
		this->lowerLayers[0]->send(this->getID(), block);
	else
		throw std::invalid_argument(" app layer must have one lower layer");
}

void AppLayer::dealReceive(int id, Block *block) {
	std::vector<unsigned char> data = block->getData();
	delete block;
	std::string str(data.begin(), data.end());
	this->log("receive data: " + str);
}
