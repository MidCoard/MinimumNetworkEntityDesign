//
// Created by 周蜀杰 on 2022/5/28.
//

#ifndef NETWORKDESIGN_ARPPACKET_H
#define NETWORKDESIGN_ARPPACKET_H


#include "network/IP.h"
#include "Packet.h"

class ARPPacket : public Packet {

public:
	ARPPacket(IP ip, IP query);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	IP ip;
	IP query;
};


#endif //NETWORKDESIGN_ARPPACKET_H
