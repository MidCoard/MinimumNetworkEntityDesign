//
// Created by 周蜀杰 on 2022/6/3.
//

#ifndef NETWORKDESIGN_UDPACKPACKET_H
#define NETWORKDESIGN_UDPACKPACKET_H


#include "Packet.h"

class UDPACKPacket : Packet {
public:
	UDPACKPacket(IP ip, IP source, int count);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	int count;
	IP ip;
	IP source;
};


#endif //NETWORKDESIGN_UDPACKPACKET_H
