//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_UDPPACKET_H
#define NETWORKDESIGN_UDPPACKET_H


#include "Packet.h"

class UDPPacket : public Packet {
public:
	UDPPacket(IP ip, IP source, unsigned char *data, int len);

	Block *createBlock(int pos);

	[[nodiscard]] int getSize() const;

	unsigned char getHeader() override;

	void init(int count);

	int getCount();

private:
	std::vector<unsigned char> data;
	int count = -1;
	IP ip;
	IP source;
	int size;

	Block *createBlock() override;

};


#endif //NETWORKDESIGN_UDPPACKET_H
