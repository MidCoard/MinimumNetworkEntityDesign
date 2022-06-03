//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_ICMPREPLYPACKET_H
#define NETWORKDESIGN_ICMPREPLYPACKET_H


#include "network/IP.h"
#include "Packet.h"

enum ICMPReplyStatus {
	kICMPReplyStatusSuccess = 0,
	kICMPReplyStatusUnreachable,
};

class ICMPReplyPacket : public Packet {

public:
	ICMPReplyPacket(IP ip, IP query, IP destination, ICMPReplyStatus status);

	Block *createBlock() override;

	unsigned char getHeader() override;

private:
	IP ip;
	IP query;
	IP destination;
	ICMPReplyStatus status;
};


#endif //NETWORKDESIGN_ICMPREPLYPACKET_H
