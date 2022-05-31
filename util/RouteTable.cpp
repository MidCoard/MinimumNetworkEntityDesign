#include "RouteTable.h"

#include <utility>

const int kMaxJumps = 10;

const long long kShortDuration = 1LL * 60 * 60 * 1000 * 1000;

const long long kLongDuration = 1000LL * kShortDuration;

std::pair<IP, int> RouteTable::lookup(const IP &ip) {
	this->check();
	std::pair<IP, int> last = {ip, -1};
	int jumps = 0;
	while (true) {
		auto it = this->next(last.first);
		jumps++;
		if (it.second == -1 || jumps > kMaxJumps)
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

void RouteTable::updateShort(const IP &ip, const IP &mask, int cost, const IP &nextHop, int id) {
	this->check();
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->table.insert(TableItem(ip, mask, cost, nextHop, id, time + kShortDuration));
}

void RouteTable::updateLong(const IP &ip, const IP &mask, int cost, const IP &nextHop, int id) {
	this->check();
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->table.insert(TableItem(ip, mask, cost, nextHop, id, time + kLongDuration));
}

std::pair<IP, int> RouteTable::next(const IP &ip) {
	for (const auto &begin: table)
		if (begin.match(ip))
			return {begin.nextHop, begin.id};
	return {BROADCAST_IP, -1};
}

bool RouteTable::TableItem::operator<(const RouteTable::TableItem &tableItem) const {
	return this->mask > tableItem.mask || (this->mask == tableItem.mask && this->cost < tableItem.cost);
}

RouteTable::TableItem::TableItem(IP ip, IP mask, int cost, IP nextHop, int id, long long time) : ip(std::move(ip)),
                                                                                                 mask(std::move(mask)),
                                                                                                 cost(cost),
                                                                                                 nextHop(std::move(
		                                                                                                 nextHop)),
                                                                                                 id(id), time(time) {}

bool RouteTable::TableItem::match(const IP &ip) const {
	return (this->ip & this->mask) == (ip & this->mask) && ip.intValue() >= this->ip.intValue();
}
