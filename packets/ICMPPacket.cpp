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
	block->writeIP(this->query);
	block->flip();
	return block;
}

unsigned char ICMPPacket::getHeader() {
	return 0x20;
}

ICMPPacket::ICMPPacket(IP ip,IP query, IP gateway) : ip(std::move(ip)), query(std::move(query)), gateway(std::move(gateway)){}
