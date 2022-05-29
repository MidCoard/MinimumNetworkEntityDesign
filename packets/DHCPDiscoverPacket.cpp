//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPDiscoverPacket.h"

#include <utility>

Block *DHCPDiscoverPacket::createBlock() {
	auto* block = new Block();
	block->writeMAC(BROADCAST_MAC);
	block->write(0);
	block->writeIP(LOCAL0);
	block->writeIP(BROADCAST_IP);
	block->writeHeader(this);
	block->writeMAC(this->mac);
	if (this->useSegment)
		block->write(1);
	else
		block->write(0);
	block->flip();
	return block;
}

unsigned char DHCPDiscoverPacket::getHeader() {
	return 0x01;
}

DHCPDiscoverPacket::DHCPDiscoverPacket(MAC mac, bool useSegment) :mac(std::move(mac)),useSegment(useSegment) {}
