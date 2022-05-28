//
// Created by 周蜀杰 on 2022/5/27.
//

#include "RouteTable.h"

#include <utility>

const int kMaxJumps = 10;

IP RouteTable::lookup(const IP& ip) {
	IP last = ip;
	int jumps = 0;
	while (true) {
		auto it = this->next(last);
		jumps++;
		if (it.isBroadcast() || jumps > kMaxJumps)
			return last;
		last = it;
	}
}

void RouteTable::check() {
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	for (auto it = this->table.begin(); it != this->table.end();)
		if (it->time < time)
			it = this->table.erase(it);
		else
			it++;
}

void RouteTable::update(const IP &ip, const IP &mask, int cost, const IP &nextHop) {
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->table.insert(TableItem(ip, mask, cost, nextHop, time + 10L * 60 * 1000 * 1000));
}

IP RouteTable::next(const IP &ip) {
	for (const auto & begin : table)
		if (begin.match(ip))
			return begin.nextHop;
	return BROADCAST_IP;
}

bool RouteTable::TableItem::operator<(const RouteTable::TableItem & tableItem) const {
	return this->mask > tableItem.mask || (this->mask == tableItem.mask && this->cost < tableItem.cost);
}

RouteTable::TableItem::TableItem(IP ip, IP mask, int cost, IP nextHop,long long time) : ip(std::move(ip)), mask(std::move(mask)), cost(cost),
                                                                         nextHop(std::move(nextHop)),time(time) {}

bool RouteTable::TableItem::match(const IP &ip) const {
	return this->ip == ip;
}
