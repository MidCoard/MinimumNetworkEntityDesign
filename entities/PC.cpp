//
// Created by 周蜀杰 on 2022/5/21.
//

#include "PC.h"

PC::PC(IP *ip, IP *gateway, MAC *mac, INetAddress *physicalAddress) : NetworkEntity(mac,physicalAddress) {
	this->layer = new AppLayer();
	auto *networkLayer = new NetworkLayer(ip, gateway);
	this->layer(networkLayer);
	auto *linkLayer = new LinkLayer(mac);
	auto *physicalLayer = new PhysicalLayer(physicalAddress);
}
