//
// Created by 周蜀杰 on 2022/6/2.
//

#ifndef NETWORKDESIGN_FRAMETABLE_H
#define NETWORKDESIGN_FRAMETABLE_H

class LinkLayer;

#include "map"
#include "Frame.h"

class FrameTable {

public:

	Block *readFrame(Block *block);

private:
	void check();

	std::map<int, std::pair<long long, std::map<int, std::vector<unsigned char>>>> table;

	Block *write(int sequence, int index, int count, unsigned char *buffer, int len, int i);
};


#endif //NETWORKDESIGN_FRAMETABLE_H
