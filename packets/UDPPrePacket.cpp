//
// Created by 周蜀杰 on 2022/5/29.
//

#include "UDPPrePacket.h"

#include <utility>

Block *UDPPrePacket::createBlock() {
	auto *block = new Block();
	block->writeIP(this->ip);
	block->write(0);
	block->writeHeader(this);
	block->writeIP(this->source);
	block->writeInt(this->count);
	block->writeInt(this->size);
	block->writeInt(this->len);
	block->flip();
	return block;
}

unsigned char UDPPrePacket::getHeader() {
	return 0x65;
}

UDPPrePacket::UDPPrePacket(IP ip, IP source, int count, int size, int len) : ip(std::move(ip)),
                                                                             source(std::move(source)), count(count),
                                                                             size(size), len(len) {}
