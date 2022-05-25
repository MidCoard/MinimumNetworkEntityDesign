//
// Created by 周蜀杰 on 2022/5/21.
//

#include "Switch.h"


Switch::Switch(int node, MAC *mac) : NetworkEntity(node, new LinkLayer(mac)) {}

std::vector<std::string> Switch::createLayers(int node, std::vector<int> ids) {
	for (int id: ids)
		this->layer->addLowerLayer(new PhysicalLayer(node, new INetAddress(generatePhysicalAddress(node, id))));
}
