//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_ICMPPACKET_H
#define NETWORKDESIGN_ICMPPACKET_H


#include "Packet.h"

class ICMPPacket : public Packet {
public:
	ICMPPacket(IP ip, IP gateway);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	IP ip;
	IP gateway;
};


#endif //NETWORKDESIGN_ICMPPACKET_H
