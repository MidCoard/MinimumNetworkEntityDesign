//
// Created by 周蜀杰 on 2022/5/29.
//

#include "ARPReplyPacket.h"

#include <utility>

unsigned char ARPReplyPacket::getHeader() {
	return 0x87;
}

Block *ARPReplyPacket::createBlock() {
	auto *block = new Block();
	block->writeMAC(mac);
	block->writeHeader(this);
	block->writeIP(ip);
	block->writeIP(reply);
	block->flip();
	return block;
}

ARPReplyPacket::ARPReplyPacket(IP ip, IP reply, MAC mac) : ip(std::move(ip)), reply(std::move(reply)),
                                                           mac(std::move(mac)) {}
