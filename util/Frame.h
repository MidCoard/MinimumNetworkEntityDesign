//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_FRAME_H
#define NETWORKDESIGN_FRAME_H

#include "vector"
#include "network/INetAddress.h"
#include "Block.h"
#include "random"
#include "bitset"

class Frame {
public:

	explicit Frame(Block* block);

	[[nodiscard]] int getSize() const;

	Block *createBlock(int pos);

	[[nodiscard]] int getStart() const;

private:
	std::vector<unsigned char> data;
	int size = 0;

	Block *recreateBlock(int sequenceNumber);

	int startSequenceNumber;
};

#endif //NETWORKDESIGN_FRAME_H
