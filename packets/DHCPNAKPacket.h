//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPNAKPACKET_H
#define NETWORKDESIGN_DHCPNAKPACKET_H


#include "network/MAC.h"
#include "network/IP.h"
#include "Packet.h"

class DHCPNAKPacket : public Packet {

public:
	DHCPNAKPacket(MAC mac, IP ip);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	MAC mac;
	IP ip;
};


#endif //NETWORKDESIGN_DHCPNAKPACKET_H
