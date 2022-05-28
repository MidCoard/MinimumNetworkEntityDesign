//
// Created by 周蜀杰 on 2022/5/27.
//

#ifndef NETWORKDESIGN_ROUTETABLE_H
#define NETWORKDESIGN_ROUTETABLE_H


#include "network/IP.h"
#include "set"

class RouteTable {

	class TableItem {

	public:
		[[nodiscard]] bool match(const IP &ip) const;

		TableItem(IP ip, IP mask, int cost, IP nextHop, long long time);

		bool operator<(const TableItem & tableItem) const;

		IP ip;
		IP mask;
		int cost;
		IP nextHop;
		long long time;
	};

public:
	IP lookup(const IP& ip);

	IP next(const IP& ip);

	void update(const IP& ip, const IP& mask, int cost, const IP& nextHop);

	void check();

private:

	std::set<TableItem> table;

};


#endif //NETWORKDESIGN_ROUTETABLE_H
