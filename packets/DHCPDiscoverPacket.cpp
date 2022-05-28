//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPDiscoverPacket.h"

Block *DHCPDiscoverPacket::createBlock() {
	auto* block = new Block();
	block->writeMAC(BROADCAST_MAC);
	block->write(0);
	block->writeIP(LOCAL0);
	block->writeIP(BROADCAST_IP);
	block->writeHeader(this);
	if (this->useSegment)
		block->write(1);
	else
		block->write(0);
	block->flip();
	return block;
}

unsigned char DHCPDiscoverPacket::getHeader() {
	return 0x11;
}

DHCPDiscoverPacket::DHCPDiscoverPacket(bool useSegment) :useSegment(useSegment) {}
