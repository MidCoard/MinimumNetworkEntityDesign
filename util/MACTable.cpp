//
// Created by 周蜀杰 on 2022/5/28.
//

#include "MACTable.h"

int MACTable::lookup(const MAC &mac) {
	if (this->table.find(mac) == this->table.end())
		return -1;
	return this->table[mac].first;
}

void MACTable::update(const MAC &mac, int port) {
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->table.insert_or_assign(mac, std::pair{port, time + 5LL * 60 * 1000 * 1000});
}

void MACTable::check() {
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	for (auto it = this->table.begin(); it != this->table.end();)
		if (it->second.second < time)
			it = this->table.erase(it);
		else
			++it;
}
