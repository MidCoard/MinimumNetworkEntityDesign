//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPACKPacket.h"

#include <utility>

Block *DHCPACKPacket::createBlock() {
	auto *block = new Block();
	block->writeMAC(this->mac);
	block->write(0);
	block->writeIP(this->ip);
	block->writeIP(this->destination);
	block->writeHeader(this);
	block->writeIP(this->destination);
	block->writeIP(this->mask);
	block->writeIP(this->gateway);
	block->writeLong(this->time);
	block->flip();
	return block;
}

unsigned char DHCPACKPacket::getHeader() {
	return 0x04;
}

DHCPACKPacket::DHCPACKPacket(MAC mac, IP ip, IP destination, IP mask, IP gateway, long long int time)
		: mac(std::move(mac)), ip(std::move(ip)), destination(std::move(destination)), mask(std::move(mask)),
		  gateway(std::move(gateway)), time(time) {}
