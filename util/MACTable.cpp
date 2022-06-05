//
// Created by 周蜀杰 on 2022/5/28.
//

#include "MACTable.h"

int MACTable::lookup(const MAC &mac) {
	int ret;
	mutex.lock();
	check();
	if (this->table.find(mac) == this->table.end())
		ret = -1;
	ret = this->table[mac].first;
	mutex.unlock();
	return ret;
}

void MACTable::update(const MAC &mac, int port) {
	this->mutex.lock();
	check();
	auto time = util::getNowTime();
	this->table.insert_or_assign(mac, std::pair{port, time + 5LL * 60 * 1000 * 1000});
	this->mutex.unlock();
}

void MACTable::check() {
	auto time = util::getNowTime();
	for (auto it = this->table.begin(); it != this->table.end();)
		if (it->second.second < time)
			it = this->table.erase(it);
		else
			++it;
}
