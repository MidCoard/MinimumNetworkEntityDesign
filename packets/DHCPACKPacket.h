//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPACKPACKET_H
#define NETWORKDESIGN_DHCPACKPACKET_H


#include "Packet.h"

class DHCPACKPacket : public Packet {

public:
	DHCPACKPacket(MAC mac, IP ip, IP destination, IP mask, IP gateway, long long int i);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	MAC mac;
	IP ip;
	IP destination;
	IP mask;
	IP gateway;
	long long int time;
};


#endif //NETWORKDESIGN_DHCPACKPACKET_H
