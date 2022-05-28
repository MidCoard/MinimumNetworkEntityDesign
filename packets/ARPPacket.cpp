//
// Created by 周蜀杰 on 2022/5/28.
//

#include "ARPPacket.h"

#include <utility>

ARPPacket::ARPPacket(IP ip, IP  query) : ip(std::move(ip)), query(std::move(query)) {}

Block *ARPPacket::createBlock() {
	auto * block = new Block();
	block->writeMAC(BROADCAST_MAC);
	block->writeHeader(this);
	block->writeIP(this->ip);
	block->writeIP(this->query);
	block->flip();
}

unsigned char ARPPacket::getHeader() {
	return 0x86; // 0x8060
}
