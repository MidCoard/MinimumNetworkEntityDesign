//
// Created by 周蜀杰 on 2022/5/21.
//

#include "PC.h"

PC::PC(IP* ip, IP* gateway, MAC* mac, INetAddress* physicalAddress) :NetworkEntity(new AppLayer())  {
	this->ip = ip;
	this->gateway = gateway;
	this->mac = mac;
	this->physicalAddress = physicalAddress;
}

PC::~PC() {
	delete ip;
	delete gateway;
}

void PC::createLayers(std::vector<int> ids) {
	if (ids.size() != 1)
		throw std::invalid_argument("PC port size != 1");
	auto* networkLayer = new NetworkLayer(ids[0],ip,gateway);
	this->layer->addLowerLayer(networkLayer);
	auto* linkLayer = new LinkLayer(*mac);
	networkLayer->addLowerLayer(linkLayer);

}
