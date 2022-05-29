//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPOfferPacket.h"

#include <utility>

unsigned char DHCPOfferPacket::getHeader() {
	return 0x02;
}

Block *DHCPOfferPacket::createBlock() {
	auto *block = new Block();
	block->writeMAC(this->mac);
	block->write(0);
	block->writeIP(this->gateway);
	block->writeIP(this->ip);
	block->writeHeader(this);
	block->writeIP(ip);
	block->writeIP(mask);
	block->writeIP(gateway);
	block->writeInt(this->dhcpID);
	block->flip();
	return block;
}

DHCPOfferPacket::DHCPOfferPacket(MAC mac, IP ip, IP mask, IP gateway, int dhcpID)
		: mac(std::move(mac)), ip(std::move(ip)), mask(std::move(mask)), gateway(std::move(gateway)), dhcpID(dhcpID) {}
