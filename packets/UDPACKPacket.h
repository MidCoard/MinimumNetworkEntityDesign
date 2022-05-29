//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_UDPACKPACKET_H
#define NETWORKDESIGN_UDPACKPACKET_H


#include "Packet.h"

class UDPACKPacket : public Packet {
public:
	UDPACKPacket(IP ip, IP source, int count,int target);
	Block * createBlock() override;
	unsigned char getHeader() override;
private:
	IP ip;
	int count;
	int target;
	IP source;
};


#endif //NETWORKDESIGN_UDPACKPACKET_H
