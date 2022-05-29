//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPDECLINEPACKET_H
#define NETWORKDESIGN_DHCPDECLINEPACKET_H


#include "Packet.h"

class DHCPDeclinePacket : public Packet{
public:
	DHCPDeclinePacket(IP ip,MAC mac, bool useSegment);

	Block * createBlock() override;
	unsigned char getHeader() override;
private:
	IP ip;
	MAC mac;
	bool useSegment;

};


#endif //NETWORKDESIGN_DHCPDECLINEPACKET_H
