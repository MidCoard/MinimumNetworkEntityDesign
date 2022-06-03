//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_UDPPREPACKET_H
#define NETWORKDESIGN_UDPPREPACKET_H


#include "Packet.h"

class UDPPrePacket : public Packet {
public:
	UDPPrePacket(IP ip, IP source, int count, int size, int len);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	IP ip;
	int count;
	int size;
	int len;
	IP source;
};


#endif //NETWORKDESIGN_UDPPREPACKET_H
