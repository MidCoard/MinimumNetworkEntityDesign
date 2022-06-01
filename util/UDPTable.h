//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_UDPTABLE_H
#define NETWORKDESIGN_UDPTABLE_H

class AppLayer;

#include "map"
#include "vector"
#include "Block.h"

class UDPTable {
public:
	explicit UDPTable(AppLayer *layer);
	int add(Block *block, int index, int size, int count, int wholeLength);
	int tryAllocate();
private:
	std::map<int,std::map<int,std::vector<unsigned char>>> table;
	int count = 0;
	std::mutex mutex;
	AppLayer *layer;
};


#endif //NETWORKDESIGN_UDPTABLE_H
