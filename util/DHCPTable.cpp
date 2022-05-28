//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPTable.h"
#include <limits>
#include <utility>

const int kApplyCount = 10;

IP DHCPTable::apply() {
	this->check();
	int rightZero = mask.getRightZero();
	if (rightZero <= 2)
		return BROADCAST_IP;
	std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> u(0, 1<<(rightZero - 1));
	IP ret = IP(ip.intValue() + u(e));
	int count = 0;
	while (!apply(ret)) {
		ret = IP(ip.intValue() + u(e));
		count++;
		if (count > kApplyCount)
			return BROADCAST_IP;
	}
	return ret;
}

bool DHCPTable::apply(IP &ip, IP &mask) {
	this->check();
	if ((mask & this->mask) != this->mask)
		return false;
	if ((ip & this->mask) != this->ip)
		return false;
	for (const auto& item : this->ips)
		if ((item.first & mask) == ip)
			return false;
	for (const auto& item : this->segments)
		if ((item.first.first & mask) == ip || (ip & item.first.second) == item.first.first)
			return false;
	for (const auto& item : this->tempIps)
		if ((item.first & mask) == ip)
			return false;
	for (const auto& item : this->tempSegments)
		if ((item.first.first & mask) == ip || (ip & item.first.second) == item.first.first)
			return false;
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->tempSegments.insert_or_assign(std::make_pair(ip, mask), time + 2LL * 60 * 1000 * 1000);
	return true;
}

void DHCPTable::check() {
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	for (auto it = this->tempSegments.begin(); it != this->tempSegments.end();)
		if (it->second < time)
			this->tempSegments.erase(it++);
		else
			it++;
	for (auto it = this->tempIps.begin(); it != this->tempIps.end();)
		if (it->second < time)
			this->tempIps.erase(it++);
		else
			it++;
}

bool DHCPTable::apply(IP &ip) {
	this->check();
	if ((ip & this->mask) != this->ip)
		return false;
	for (const auto& item : this->ips)
		if (item.first == ip)
			return false;
	for (const auto& item : this->segments)
		if ((ip & item.first.second) == item.first.first)
			return false;
	for (const auto& item : this->tempIps)
		if (item.first == ip)
			return false;
	for (const auto& item : this->tempSegments)
		if ((ip & item.first.second) == item.first.first)
			return false;
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->tempIps.insert_or_assign(ip, time + 2LL * 60 * 1000 * 1000);
	return true;
}

std::pair<IP, IP> DHCPTable::applySegment() {
	this->check();
	if (ip.)
}

DHCPTable::DHCPTable(IP ip, IP mask) : ip(std::move(ip)), mask(std::move(mask)) {}