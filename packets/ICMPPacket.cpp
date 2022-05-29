//
// Created by 周蜀杰 on 2022/5/29.
//

#include "ICMPPacket.h"

#include <utility>

Block *ICMPPacket::createBlock() {
	auto *block = new Block();
	block->writeIP(this->gateway);
	block->writeHeader(this);
	block->writeIP(this->ip);
	if (ip == gateway)
		block->write(1);
	else block->write(0);
	block->flip();
	return block;
}

unsigned char ICMPPacket::getHeader() {
	return 0x20;
}

ICMPPacket::ICMPPacket(IP ip, IP gateway) : ip(std::move(ip)), gateway(std::move(gateway)) {}
