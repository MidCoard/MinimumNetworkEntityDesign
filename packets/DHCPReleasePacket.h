//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPRELEASEPACKET_H
#define NETWORKDESIGN_DHCPRELEASEPACKET_H


#include "Packet.h"

class DHCPReleasePacket : public Packet {

public:

	DHCPReleasePacket(IP segment, IP mask, IP gateway, MAC mac,MAC source, bool useSegment);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:

	IP segment;
	IP mask;
	IP gateway;
	MAC mac;
	bool useSegment;
	MAC source;
};


#endif //NETWORKDESIGN_DHCPRELEASEPACKET_H
