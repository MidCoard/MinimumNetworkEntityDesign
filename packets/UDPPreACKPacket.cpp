//
// Created by 周蜀杰 on 2022/5/29.
//

#include "UDPPreACKPacket.h"

#include <utility>

unsigned char UDPPreACKPacket::getHeader() {
	return 0x66;
}

Block *UDPPreACKPacket::createBlock() {
	auto *block = new Block();
	block->writeIP(this->ip);
	block->write(0);
	block->writeHeader(this);
	block->writeIP(this->source);
	block->writeInt(this->count);
	block->writeInt(this->target);
	block->flip();
	return block;
}

UDPPreACKPacket::UDPPreACKPacket(IP ip, IP source, int count, int target) : ip(std::move(ip)),
                                                                            source(std::move(source)), count(count),
                                                                            target(target) {}
