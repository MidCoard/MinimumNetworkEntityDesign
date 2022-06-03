//
// Created by 周蜀杰 on 2022/6/3.
//

#ifndef NETWORKDESIGN_UDPREQUESTPACKET_H
#define NETWORKDESIGN_UDPREQUESTPACKET_H


#include "Packet.h"

class UDPRequestPacket : public Packet {
public:
	Block * createBlock() override;

	unsigned char getHeader() override;
};


#endif //NETWORKDESIGN_UDPREQUESTPACKET_H
