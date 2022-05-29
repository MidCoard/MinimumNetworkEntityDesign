//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPDeclinePacket.h"

#include <utility>

Block *DHCPDeclinePacket::createBlock() {
	auto block = new Block();
	block->writeMAC(BROADCAST_MAC);
	block->write(0);
	block->writeIP(LOCAL0);
	block->writeIP(BROADCAST_IP);
	block->writeHeader(this);
	block->writeIP(this->ip);
	block->write(this->useSegment);
	block->flip();
	return block;
}

unsigned char DHCPDeclinePacket::getHeader() {
	return 0x06;
}

DHCPDeclinePacket::DHCPDeclinePacket(IP ip,MAC mac, bool useSegment) : ip(std::move(ip)),mac(std::move(mac)), useSegment(useSegment) {}
