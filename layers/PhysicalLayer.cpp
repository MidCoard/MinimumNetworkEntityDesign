//
// Created by 周蜀杰 on 2022/5/9.
//

#include "PhysicalLayer.h"

#include <utility>

PhysicalLayer::PhysicalLayer(INetAddress* iNetAddress) : PhysicalLayer(-1, iNetAddress) {
}

std::string PhysicalLayer::getName() {
	return "PHY";
}

PhysicalLayer::PhysicalLayer(int id, INetAddress *iNetAddress) : Layer(id), iNetAddress(iNetAddress) {
}
