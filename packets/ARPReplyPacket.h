//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_ARPREPLYPACKET_H
#define NETWORKDESIGN_ARPREPLYPACKET_H


#include "Packet.h"

class ARPReplyPacket : Packet {
public:
	ARPReplyPacket(IP ip, IP reply, MAC mac);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	IP ip;
	IP reply;
	MAC mac;
};


#endif //NETWORKDESIGN_ARPREPLYPACKET_H
