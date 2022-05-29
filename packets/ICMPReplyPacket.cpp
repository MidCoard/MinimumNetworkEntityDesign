//
// Created by 周蜀杰 on 2022/5/29.
//

#include "ICMPReplyPacket.h"

#include <utility>

ICMPReplyPacket::ICMPReplyPacket(IP ip,ICMPReplyStatus status) : ip(std::move(ip)),status(status) {}

Block *ICMPReplyPacket::createBlock() {
	auto* block = new Block();
	block->writeIP(ip);
	block->writeHeader(this);
	block->write(status);
	block->flip();
	return block;
}

unsigned char ICMPReplyPacket::getHeader() {
	return 0x21;
}
