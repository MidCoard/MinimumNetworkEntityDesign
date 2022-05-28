//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPREQUESTPACKET_H
#define NETWORKDESIGN_DHCPREQUESTPACKET_H


#include "Packet.h"

class DHCPRequestPacket : public Packet {
public:
	DHCPRequestPacket(IP segment, IP mask, bool useSegment);

	Block * createBlock() override;

	unsigned char getHeader() override;
private:
	IP segment;
	IP mask;
	bool useSegment;
};


#endif //NETWORKDESIGN_DHCPREQUESTPACKET_H
