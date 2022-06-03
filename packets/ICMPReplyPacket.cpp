//
// Created by 周蜀杰 on 2022/5/29.
//

#include "ICMPReplyPacket.h"

#include <utility>

ICMPReplyPacket::ICMPReplyPacket(IP ip, IP query, IP destination, ICMPReplyStatus status) : ip(std::move(ip)),
                                                                                            query(std::move(query)),
                                                                                            destination(std::move(
		                                                                                            destination)),
                                                                                            status(status) {}

Block *ICMPReplyPacket::createBlock() {
	auto *block = new Block();
	block->writeIP(destination);
	block->writeHeader(this);
	block->writeIP(this->ip);
	block->writeIP(this->query);
	block->write(status);
	block->flip();
	return block;
}

unsigned char ICMPReplyPacket::getHeader() {
	return 0x21;
}
