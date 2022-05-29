//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPDISCOVERPACKET_H
#define NETWORKDESIGN_DHCPDISCOVERPACKET_H


#include "Packet.h"

class DHCPDiscoverPacket : public Packet {
public:
	DHCPDiscoverPacket(MAC mac, bool useSegment);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	bool useSegment;
	MAC mac;
};


#endif //NETWORKDESIGN_DHCPDISCOVERPACKET_H
