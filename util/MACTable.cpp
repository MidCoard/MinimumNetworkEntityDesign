//
// Created by 周蜀杰 on 2022/5/28.
//

#include "MACTable.h"

int MACTable::lookup(const MAC& mac) {
	if (this->macTable.find(mac) == this->macTable.end())
		return -1;
	return this->macTable[mac].first;
}

void MACTable::update(const MAC& mac, int port) {
	auto time = std::chrono::system_clock::now();
	this->macTable.insert_or_assign(mac, std::pair{port , time.time_since_epoch().count() + 2L * 60 * 1000 * 1000});
}

void MACTable::check() {
	auto time = std::chrono::system_clock::now();
	for (auto it = this->macTable.begin(); it != this->macTable.end();)
		if (it->second.second < time.time_since_epoch().count())
			it = this->macTable.erase(it);
		else
			++it;
}
