//
// Created by 周蜀杰 on 2022/5/28.
//

#include "ARPTable.h"

const long long kARPTime = 5LL * 60 * 1000 * 1000;

MAC ARPTable::lookup(const IP &ip) {
	mutex.lock();
	if (this->table.find(ip) == this->table.end()) {
		mutex.unlock();
		return BROADCAST_MAC;
	}
	MAC mac = this->table.at(ip).first;
	mutex.unlock();
	return mac;
}

void ARPTable::update(const IP &ip, const MAC &mac) {
	mutex.lock();
	this->check();
	auto time = util::getNowTime();
	this->table.insert_or_assign(ip, std::pair{mac, time + kARPTime});
	mutex.unlock();
}

void ARPTable::check() {
	auto time = util::getNowTime();
	for (auto it = this->table.begin(); it != this->table.end();)
		if (it->second.second < time)
			it = this->table.erase(it);
		else
			++it;
}


