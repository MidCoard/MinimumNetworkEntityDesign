//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPTable.h"
#include <limits>
#include <utility>

const int kApplyCount = 10;
const long long int kDHCPTime = 2LL * 60 * 60 * 1000 * 1000;

IP DHCPTable::apply() {
	this->check();
	int rightZero = mask.getRightZero();
	if (rightZero <= 2)
		return BROADCAST_IP;
	std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> u(0, (1<<(rightZero - 1)) - 1);
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

bool DHCPTable::apply(const IP &ip,const IP &mask) {
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
	this->tempSegments.insert_or_assign(std::pair(ip, mask), std::pair{time + 2LL * 60 * 1000 * 1000, dhcpID});
	return true;
}

void DHCPTable::check() {
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	for (auto it = this->segments.begin(); it != this->segments.end();)
		if (it->second < time)
			this->segments.erase(it++);
		else
			it++;
	for (auto it = this->ips.begin(); it != this->ips.end();)
		if (it->second < time)
			this->ips.erase(it++);
		else
			it++;
	for (auto it = this->tempSegments.begin(); it != this->tempSegments.end();)
		if (it->second.first < time)
			this->tempSegments.erase(it++);
		else
			it++;
	for (auto it = this->tempIps.begin(); it != this->tempIps.end();)
		if (it->second.first < time)
			this->tempIps.erase(it++);
		else
			it++;
}

bool DHCPTable::apply(const IP &ip) {
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
	this->tempIps.insert_or_assign(ip, std::pair{time + 2LL * 60 * 1000 * 1000,dhcpID});
	return true;
}

std::pair<IP, IP> DHCPTable::applySegment() {
	this->check();
	int rightZero = mask.getRightZero();
	if (rightZero <= 4)
		return std::make_pair(BROADCAST_IP, BROADCAST_IP);
	std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> u(0, 15);
	IP retMask = IP(mask.intValue() + (15u << (rightZero - 4)));
	IP ret = IP(ip.intValue() + (u(e) << (rightZero - 4)));
	int count = 0;
	while (!apply(ret, retMask)) {
		ret = IP(ip.intValue() + (u(e) << (rightZero - 4)));
		count++;
		if (count > kApplyCount)
			return std::make_pair(BROADCAST_IP, BROADCAST_IP);
	}
	return std::make_pair(ret, retMask);
}

DHCPTable::DHCPTable(IP ip, IP mask) : ip(std::move(ip)), mask(std::move(mask)) {}

bool DHCPTable::tryApply(const IP& ip,const IP& mask, int dhcpID) {
	this->check();
	if (dhcpID == -1)
		return applyDirect(ip, mask);
	else
		for (const auto& item : this->tempSegments)
			if (item.second.second == dhcpID) {
				auto time = std::chrono::system_clock::now().time_since_epoch().count();
				this->segments.insert_or_assign(std::pair{item.first.first, item.first.second}, time + 2L * 60 * 60 * 1000 * 1000);
				this->tempSegments.erase(item.first);
				return true;
			}
	return false;
}

bool DHCPTable::applyDirect(const IP &ip,const IP &mask) {
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
	this->segments.insert_or_assign(std::pair(ip, mask), time + 2L * 60 * 60 * 1000 * 1000);
	return true;
}

bool DHCPTable::tryApply(const IP& ip, int dhcpID) {
	this->check();
	if (dhcpID == -1)
		return applyDirect(ip);
	else
		for (const auto& item : this->tempIps)
			if (item.second.second == dhcpID) {
				auto time = std::chrono::system_clock::now().time_since_epoch().count();
				this->ips.insert_or_assign(item.first, time + 2L * 60 * 60 * 1000 * 1000);
				this->tempIps.erase(item.first);
				return true;
			}
	return false;
}

bool DHCPTable::applyDirect(const IP& ip) {
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
	this->ips.insert_or_assign(ip, time + kDHCPTime);
	return true;
}

bool DHCPTable::renewal(const IP& ip, const IP& mask) {
	this->check();
	auto time = std::chrono::system_clock::now().time_since_epoch().count() + kDHCPTime;
	for (auto& item : this->segments)
		if ((ip & item.first.second) == item.first.first) {
			item.second = time;
			return true;
		}
	this->ips.insert_or_assign(ip, time);
	return true;
	// always return true?
}

bool DHCPTable::renewal(const IP& ip) {
	this->check();
	auto time = std::chrono::system_clock::now().time_since_epoch().count() + kDHCPTime;
	for (auto& item : this->ips)
		if (item.first == ip) {
			item.second = time;
			return true;
		}
	this->ips.insert_or_assign(ip, time);
	return true;
	// always return true?
}
