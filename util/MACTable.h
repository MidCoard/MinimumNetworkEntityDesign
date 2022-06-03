//
// Created by 周蜀杰 on 2022/5/28.
//

#ifndef NETWORKDESIGN_MACTABLE_H
#define NETWORKDESIGN_MACTABLE_H


#include "network/MAC.h"
#include "map"

class MACTable {

public:

	int lookup(const MAC &mac);

	void update(const MAC &mac, int port);


private:
	void check();

	std::map<MAC, std::pair<int, long long>> table;
	std::mutex mutex;
};


#endif //NETWORKDESIGN_MACTABLE_H
