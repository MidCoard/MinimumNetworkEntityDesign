//
// Created by 周蜀杰 on 2022/5/9.
//

#include "PhysicalLayer.h"

PhysicalLayer::PhysicalLayer(INetAddress *iNetAddress) {
	this->iNetAddress = iNetAddress;
}

std::string PhysicalLayer::getName() {
	return "PHY";
}
