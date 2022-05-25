//
// Created by 周蜀杰 on 2022/5/21.
//

#include "PC.h"

PC::PC(IP* ip, IP* gateway, MAC* mac, INetAddress* physicalAddress) : NetworkEntity(mac,physicalAddress) ,ip(ip), gateway(gateway) {}

PC::~PC() {
	delete ip;
	delete gateway;
}