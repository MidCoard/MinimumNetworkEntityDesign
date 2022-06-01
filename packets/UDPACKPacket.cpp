//
// Created by 周蜀杰 on 2022/5/29.
//

#include "UDPACKPacket.h"

#include <utility>

unsigned char UDPACKPacket::getHeader() {
	return 0x66;
}

Block *UDPACKPacket::createBlock() {
	auto* block = new Block();
	block->writeIP(this->ip);
	block->write(0);
	block->writeHeader(this);
	block->writeIP(this->source);
	block->writeInt(this->count);
	block->writeInt(this->target);
	block->flip();
	return block;
}

UDPACKPacket::UDPACKPacket(IP ip,IP source, int count, int target) : ip(std::move(ip)),source(std::move(source)), count(count), target(target) {}
