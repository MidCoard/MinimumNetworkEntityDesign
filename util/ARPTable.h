//
// Created by 周蜀杰 on 2022/5/28.
//

#ifndef NETWORKDESIGN_ARPTABLE_H
#define NETWORKDESIGN_ARPTABLE_H


#include "network/MAC.h"
#include "network/IP.h"
#include "map"

class ARPTable {
public:

	MAC lookup(const IP &ip);

	void update(const IP &ip, const MAC &mac);

	void check();

private:
	std::map<IP, std::pair<MAC, long long>> table;
	std::mutex mutex;
};


#endif //NETWORKDESIGN_ARPTABLE_H
