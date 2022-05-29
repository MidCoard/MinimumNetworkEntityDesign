//
// Created by 周蜀杰 on 2022/5/27.
//

#ifndef NETWORKDESIGN_BLOCK_H
#define NETWORKDESIGN_BLOCK_H

class Packet;

#include "vector"
#include "network/MAC.h"
#include "network/IP.h"
#include "Packet.h"

class Block {
public:
	Block();

	explicit Block(int sendCount);

	explicit Block(Block *block);

	void flip();

	void write(unsigned char *data, int len);

	[[nodiscard]] int getRemaining() const;

	int read(unsigned char *data, int len);

	std::vector<unsigned char> read();

	MAC readMAC();

	void writeBlock(Block *block);

	void writeMAC(const MAC& mac);

	void write(const std::vector<unsigned char>& data);

	void write(unsigned char c);

	IP readIP();

	void writeIP(const IP& ip);

	void writeHeader(Packet *packet);

	[[nodiscard]] int getSendCount() const;

	Block *copy();

	void writeInt(int i);

	int readInt();

	long long int readLong();

	void writeLong(long long int i);

private:
	// avoid use pointer
	std::vector<unsigned char> temp;
	int remaining;
	int pos;
	int sendCount;
};


#endif //NETWORKDESIGN_BLOCK_H
