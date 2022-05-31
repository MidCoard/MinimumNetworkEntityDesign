//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPRequestPacket.h"

#include <utility>

Block *DHCPRequestPacket::createBlock() {
	auto *block = new Block();
	block->writeMAC(this->target);
	block->write(0);
	if (this->gateway.isBroadcast())
		block->writeIP(LOCAL0);
	else
		block->writeIP(this->segment);
	block->writeIP(this->gateway);
	block->writeHeader(this);
	block->writeIP(this->segment);
	block->writeIP(this->mask);
	block->writeMAC(this->mac);
	block->writeInt(dhcpID);
	block->write(useSegment);
	block->flip();
	return block;
}

unsigned char DHCPRequestPacket::getHeader() {
	return 0x03;
}

DHCPRequestPacket::DHCPRequestPacket(IP segment, IP mask, MAC mac, int dhcpID, bool useSegment) : segment(
		std::move(segment)), mask(std::move(mask)),
                                                                                                  mac(std::move(mac)),
                                                                                                  target(BROADCAST_MAC),
                                                                                                  dhcpID(dhcpID),
                                                                                                  useSegment(
		                                                                                                  useSegment),
                                                                                                  gateway(BROADCAST_IP) {}

DHCPRequestPacket::DHCPRequestPacket(IP segment, IP mask, IP gateway, MAC mac,MAC target, bool useSegment) : segment(
		std::move(segment)), mask(std::move(mask)),
                                                                                                  gateway(std::move(
		                                                                                                  gateway)),
                                                                                                  mac(std::move(mac)),
																								  target(std::move(target)),
                                                                                                  useSegment(
		                                                                                                  useSegment),
                                                                                                  dhcpID(-1) {}
