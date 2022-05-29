//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPNAKPacket.h"

#include <utility>

DHCPNAKPacket::DHCPNAKPacket(MAC mac, IP ip) : mac(std::move(mac)), ip(std::move(ip)) {}

Block *DHCPNAKPacket::createBlock() {
	auto *block = new Block();
	block->writeMAC(this->mac);
	block->write(0);
	block->writeIP(this->ip);
	block->writeIP(BROADCAST_IP);
	block->writeHeader(this);
	block->flip();
	return block;
}

unsigned char DHCPNAKPacket::getHeader() {
	return 0x05;
}
