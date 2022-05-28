//
// Created by 周蜀杰 on 2022/5/27.
//

#ifndef NETWORKDESIGN_ROUTETABLE_H
#define NETWORKDESIGN_ROUTETABLE_H


#include "network/IP.h"

class RouteTable {

public:
	int lookup(IP ip);

	void check();
};


#endif //NETWORKDESIGN_ROUTETABLE_H
