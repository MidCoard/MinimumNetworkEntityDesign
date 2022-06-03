//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_UDPTABLE_H
#define NETWORKDESIGN_UDPTABLE_H

class AppLayer;

#include "map"
#include "vector"
#include "Block.h"
#include "BlockingCollection.h"

class UDPTable {
public:
	explicit UDPTable(AppLayer *layer);

	int add(Block *block, const IP &ip, int count, int index);

	int tryAllocate(const IP &ip, int count, int size, int wholeLength);

	bool ack(const IP &ip, int count);

private:
	std::map<std::pair<IP, int>, std::pair<int, std::pair<int, int>>> idTable;
	std::map<int, std::map<int, std::vector<unsigned char>>> table;
	std::map<std::pair<IP, int>, int> resendTable;
	std::mutex mutex;
	int count = 0;
	AppLayer *layer;

};


#endif //NETWORKDESIGN_UDPTABLE_H
