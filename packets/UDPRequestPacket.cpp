//
// Created by 周蜀杰 on 2022/6/3.
//

#include "UDPRequestPacket.h"

Block *UDPRequestPacket::createBlock() {
	return nullptr;
}

unsigned char UDPRequestPacket::getHeader() {
	return 0x91;
}
