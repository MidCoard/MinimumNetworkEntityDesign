//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPOFFERPACKET_H
#define NETWORKDESIGN_DHCPOFFERPACKET_H


#include "Packet.h"

class DHCPOfferPacket : public Packet {

public:
	DHCPOfferPacket(MAC mac, IP ip, IP mask, IP gateway, int dhcpID);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	MAC mac;
	IP ip;
	IP mask;
	IP gateway;
	int dhcpID;
};


#endif //NETWORKDESIGN_DHCPOFFERPACKET_H
