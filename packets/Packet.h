//
// Created by 周蜀杰 on 2022/5/28.
//

#ifndef NETWORKDESIGN_PACKET_H
#define NETWORKDESIGN_PACKET_H

class Block;

#include "Block.h"

class Packet {
public:
	virtual Block* createBlock() = 0;

	virtual unsigned char getHeader() = 0;
};


#endif //NETWORKDESIGN_PACKET_H
