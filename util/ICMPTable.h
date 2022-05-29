//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_ICMPTABLE_H
#define NETWORKDESIGN_ICMPTABLE_H


#include "network/IP.h"
#include "BlockingCollection.h"
#include "thread"
#include "map"

class ICMPTable {
public:

	void add(const IP& ip, const IP& query);

	void remove(const IP& ip,const IP& query);

	void update(const IP& ip, const IP& query, bool flag);

	int lookup(const IP& ip, const IP& query);

private:
	// multithreading safe should be considered
	std::map<std::pair<IP,IP>,int> map;
	std::mutex mtx;
};


#endif //NETWORKDESIGN_ICMPTABLE_H
