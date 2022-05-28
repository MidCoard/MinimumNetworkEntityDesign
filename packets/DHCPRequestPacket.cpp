//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPRequestPacket.h"

#include <utility>

Block *DHCPRequestPacket::createBlock() {
	auto* block = new Block();
	block->writeMAC(BROADCAST_MAC);
	block->write(0);
	block->writeIP(LOCAL0);
	block->writeIP(BROADCAST_IP);
	block->writeHeader(this);
	block->writeIP(this->segment);
	block->writeIP(this->mask);
	if (this->useSegment)
		block->write(1);
	else
		block->write(0);
	block->flip();
	return block;
}

unsigned char DHCPRequestPacket::getHeader() {
	return 0x03;
}

DHCPRequestPacket::DHCPRequestPacket(IP segment, IP mask, bool useSegment) : segment(std::move(segment)), mask(std::move(mask)), useSegment(useSegment) {}
