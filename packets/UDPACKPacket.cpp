//
// Created by 周蜀杰 on 2022/6/3.
//

#include "UDPACKPacket.h"

#include <utility>

unsigned char UDPACKPacket::getHeader() {
	return 0x90;
}

Block *UDPACKPacket::createBlock() {
	auto *block = new Block();
	block->writeIP(this->ip);
	block->write(0);
	block->writeHeader(this);
	block->writeIP(this->source);
	block->writeInt(count);
	block->flip();
	return block;
}

UDPACKPacket::UDPACKPacket(IP ip, IP source, int count) : ip(std::move(ip)), count(count), source(std::move(source)) {}
