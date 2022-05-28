//
// Created by 周蜀杰 on 2022/5/27.
//

#ifndef NETWORKDESIGN_BLOCK_H
#define NETWORKDESIGN_BLOCK_H

#include "vector"
#include "network/MAC.h"
#include "network/IP.h"

class Block {
public:
	Block();

	Block(Block *block);

	void flip();

	void write(unsigned char *data, int len);

	int getRemaining() const;

	int read(unsigned char *data, int len);

	std::vector<unsigned char> read();

	MAC readMAC();

	void writeBlock(Block *block);

	void writeMAC(const MAC& mac);

	void write(const std::vector<unsigned char>& data);

	IP readIP();

	void writeIP(const IP& ip);

private:
	// avoid use pointer
	std::vector<unsigned char> temp;
	int remaining;
	int pos;
};


#endif //NETWORKDESIGN_BLOCK_H
