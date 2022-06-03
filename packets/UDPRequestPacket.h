//
// Created by 周蜀杰 on 2022/6/3.
//

#ifndef NETWORKDESIGN_UDPREQUESTPACKET_H
#define NETWORKDESIGN_UDPREQUESTPACKET_H


#include "Packet.h"

class UDPRequestPacket : public Packet {
public:
	UDPRequestPacket(IP ip, IP source, int count, std::vector<int> ids);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	IP ip;
	IP source;
	int count;
	std::vector<int> ids;
};


#endif //NETWORKDESIGN_UDPREQUESTPACKET_H
