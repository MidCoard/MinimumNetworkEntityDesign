//
// Created by 周蜀杰 on 2022/5/28.
//

#include "ARPTable.h"

MAC ARPTable::lookup(const IP &ip) {
	if (this->table.find(ip) == this->table.end())
		return BROADCAST_MAC;
	return this->table.at(ip).first;
}

void ARPTable::update(const IP &ip, const MAC &mac) {
	this->check();
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->table.insert_or_assign(ip, std::pair{mac, time + 2LL * 60 * 1000 * 1000});
}

void ARPTable::check() {
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	for (auto it = this->table.begin(); it != this->table.end();)
		if (it->second.second < time)
			it = this->table.erase(it);
		else
			++it;
}


