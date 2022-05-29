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
private:
	std::vector<unsigned char> data;
	int count;
	IP ip;
	Block **blocks = nullptr;
	int size;
};


#endif //NETWORKDESIGN_UDPPACKET_H
