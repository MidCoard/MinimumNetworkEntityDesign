//
// Created by 周蜀杰 on 2022/6/3.
//

#include "UDPRequestPacket.h"

#include <utility>

Block *UDPRequestPacket::createBlock() {
	auto *block = new Block();
	block->writeIP(this->ip);
	block->write(0);
	block->writeHeader(this);
	block->writeIP(this->source);
	block->writeInt(count);
	for (int id: this->ids)
		block->writeInt(id);
	block->flip();
	return block;
}

unsigned char UDPRequestPacket::getHeader() {
	return 0x91;
}

UDPRequestPacket::UDPRequestPacket(IP ip, IP source, int count, std::vector<int> ids) : ip(std::move(ip)),
                                                                                        source(std::move(source)),
                                                                                        count(count),
                                                                                        ids(std::move(ids)) {}
