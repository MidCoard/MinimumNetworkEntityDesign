//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_UDPPRETABLE_H
#define NETWORKDESIGN_UDPPRETABLE_H
class AppLayer;

#include "map"
#include "vector"
#include "network/IP.h"

class UDPPreTable {

public:
	explicit UDPPreTable(AppLayer* layer);
	int tryAllocate(unsigned char* data, int len);
	void send(int count, int target, const IP& ip);

private:
	int count = 0;
	std::map<int,std::vector<unsigned char>> table;
	std::mutex mutex;
	AppLayer *layer;
};


#endif //NETWORKDESIGN_UDPPRETABLE_H
