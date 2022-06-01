//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_UDPPACKET_H
#define NETWORKDESIGN_UDPPACKET_H


#include "Packet.h"

class UDPPacket : public Packet {
public:
	UDPPacket(IP ip,int count);
	Block * createBlock() override;
	[[nodiscard]] int getSize() const;
	unsigned char getHeader() override;
	void write(const std::vector<unsigned char>& data);

	void init();

private:
	std::vector<unsigned char> data;
	int count;
	IP ip;
	int offset = 0;
	int size = 0;
	int pos = 0;
};


#endif //NETWORKDESIGN_UDPPACKET_H
