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

	void add(Frame *frame);

	Block *readFrame(Block *block);

private:
	std::map<int, std::pair<Frame *,long long>> frameTable;

	void check();

	Frame *get(int start);
	std::map<int,std::pair<long long, std::map<int,std::vector<unsigned char>>>> table;

	Block * write(int sequence, int index, int count, unsigned char *buffer, int len);
};


#endif //NETWORKDESIGN_FRAMETABLE_H
