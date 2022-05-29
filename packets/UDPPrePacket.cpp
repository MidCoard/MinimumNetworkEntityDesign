//
// Created by 周蜀杰 on 2022/5/29.
//

#include "UDPPrePacket.h"

#include <utility>

Block *UDPPrePacket::createBlock() {
	auto *block = new Block();
	block->writeIP(this->ip);
	block->writeHeader(this);
	block->writeIP(this->ip);
	block->writeInt(this->count);
	block->flip();
	return block;
}

unsigned char UDPPrePacket::getHeader() {
	return 0x65;
}

UDPPrePacket::UDPPrePacket(IP ip,int count) :ip(std::move(ip)),count(count) {}
