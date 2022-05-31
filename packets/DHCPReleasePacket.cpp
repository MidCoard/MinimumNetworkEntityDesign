//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPReleasePacket.h"

#include <utility>

unsigned char DHCPReleasePacket::getHeader() {
	return 0x07;
}

Block *DHCPReleasePacket::createBlock() {
	auto *block = new Block();
	block->writeMAC(mac);
	block->write(0);
	block->writeIP(this->segment);
	block->writeIP(this->gateway);
	block->writeHeader(this);
	block->writeIP(this->segment);
	block->writeIP(this->mask);
	block->writeMAC(this->source);
	block->write(useSegment);
	block->flip();
	return block;
}

DHCPReleasePacket::DHCPReleasePacket(IP segment, IP mask, IP gateway, MAC mac,MAC source, bool useSegment) : segment(
		std::move(segment)), mask(std::move(mask)), gateway(std::move(gateway)), mac(std::move(mac)), source(std::move(source)), useSegment(
		useSegment) {}
