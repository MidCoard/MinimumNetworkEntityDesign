//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPTable.h"
#include <limits>
#include <utility>

const int kApplyCount = 100;
const long long int kDHCPTime = 2LL * 60 * 60 * 1000 * 1000;


// use sequence to avoid the same ip segment
std::pair<IP,long long int> DHCPTable::apply() {
	this->check();
	TableItem item(this->nowCount, 1);
	auto it = this->segments.upper_bound(item);
	if (it != this->segments.begin()) {
		it--;
		if (it->first.right() >= item.left())
			return {BROADCAST_IP, -1};
	}
	auto it2 = this->tempSegments.upper_bound(item);
	if (it2 != this->tempSegments.begin()) {
		it2--;
		if (it2->first.right() >= item.left())
			return {BROADCAST_IP, -1};
	}
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->tempSegments.insert_or_assign(item, std::pair{time + 2LL * 60 * 1000 * 1000, dhcpID});
	long long int count = this->nowCount++;
	if (this->nowCount >= this->maxCount)
		this->nowCount = 0;
	return {IP(this->ip.intValue() + count), this->dhcpID++};
}

unsigned long long DHCPTable::apply(const IP& ip) {
	this->check();
	if ((ip & this->mask) != this->ip || ip.intValue() < this->ip.intValue())
		return  -1;
	TableItem item(ip.intValue() - this->ip.intValue(), 1);
	auto it = this->segments.upper_bound(item);
	if (it != this->segments.begin()) {
		it--;
		if (it->first.right() >= item.left())
			return -1;
	}
	auto it2 = this->tempSegments.upper_bound(item);
	if (it2 != this->tempSegments.begin()) {
		it2--;
		if (it2->first.right() >= item.left())
			return -1;
	}
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->tempSegments.insert_or_assign(item, std::pair{time + 2LL * 60 * 1000 * 1000, dhcpID});
	return this->dhcpID++;
}

long long int DHCPTable::apply(const IP &ip, const IP &mask) {
	this->check();
	if ((mask & this->mask) != this->mask)
		return -1;
	if ((ip & this->mask) != this->ip)
		return -1;
	if (ip.intValue() < this->ip.intValue())
		return -1;
	int zeros = mask.getRightZero();
	TableItem item(ip.intValue() - this->ip.intValue(), (1LL<<zeros) - (this->ip.intValue() - (this->ip & this->mask).intValue()) + 1);
	auto it = this->segments.upper_bound(item);
	if (it != this->segments.begin()) {
		it--;
		if (it->first.right() >= item.left())
			return -1;
		it++;
		if (it != this->segments.end() && it->first.left() <= item.right())
			return -1;
	}
	auto it2 = this->tempSegments.upper_bound(item);
	if (it2 != this->tempSegments.begin()) {
		it2--;
		if (it2->first.right() >= item.left())
			return -1;
		it2++;
		if (it2 != this->tempSegments.end() && it2->first.left() <= item.right())
			return -1;
	}
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->tempSegments.insert_or_assign(item, std::pair{time + 2LL * 60 * 1000 * 1000, dhcpID});
	return dhcpID++;
}

std::pair<std::pair<IP,IP>,long long int> DHCPTable::applySegment() {
	this->check();
	int zeros = this->mask.getRightZero();
	if (zeros < 4)
		return {std::pair{BROADCAST_IP, BROADCAST_IP}, -1};
	TableItem item(this->nowCount, 1LL << 4);
	auto it = this->segments.upper_bound(item);
	if (it != this->segments.begin()) {
		it--;
		if (it->first.right() >= item.left())
			return {std::pair{BROADCAST_IP, BROADCAST_IP}, -1};
		it++;
		if (it != this->segments.end() && it->first.left() <= item.right())
			return {std::pair{BROADCAST_IP, BROADCAST_IP}, -1};
	}
	auto it2 = this->tempSegments.upper_bound(item);
	if (it2 != this->tempSegments.begin()) {
		it2--;
		if (it2->first.right() >= item.left())
			return {std::pair{BROADCAST_IP, BROADCAST_IP}, -1};
		it2++;
		if (it2 != this->tempSegments.end() && it2->first.left() <= item.right())
			return {std::pair{BROADCAST_IP, BROADCAST_IP}, -1};
	}
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->tempSegments.insert_or_assign(item, std::pair{time + 2LL * 60 * 1000 * 1000, dhcpID});
	long long int count = this->nowCount;
	this->nowCount += (1LL << 4);
	if (this->nowCount >= this->maxCount)
		this->nowCount = 0;
	return {{IP(this->ip.intValue() + count),IP(0xffu,0xffu,0xffu,0xf0u)},this->dhcpID++};
}

void DHCPTable::check() {
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	for (auto it = this->segments.begin(); it != this->segments.end();)
		if (it->second.first < time)
			this->segments.erase(it++);
		else
			it++;
	for (auto it = this->tempSegments.begin(); it != this->tempSegments.end();)
		if (it->second.first < time)
			this->tempSegments.erase(it++);
		else
			it++;
}

DHCPTable::DHCPTable(IP ip, IP mask) : ip(std::move(ip)), mask(std::move(mask)) {
	int zeros = this->mask.getRightZero();
	this->maxCount = (1LL<<zeros) - (this->ip.intValue() - (this->ip & this->mask).intValue());
}

std::pair<IP, IP> DHCPTable::directApplySegment(const MAC& mac) {
	this->check();
	int zeros = this->mask.getRightZero();
	if (zeros < 4)
		return {std::pair{BROADCAST_IP, BROADCAST_IP}};
	TableItem item(this->nowCount, 1LL << 4);
	auto it = this->segments.upper_bound(item);
	if (it != this->segments.begin()) {
		it--;
		if (it->first.right() >= item.left())
			return {std::pair{BROADCAST_IP, BROADCAST_IP}};
		it++;
		if (it != this->segments.end() && it->first.left() <= item.right())
			return {std::pair{BROADCAST_IP, BROADCAST_IP}};
	}
	auto it2 = this->tempSegments.upper_bound(item);
	if (it2 != this->tempSegments.begin()) {
		it2--;
		if (it2->first.right() >= item.left())
			return {std::pair{BROADCAST_IP, BROADCAST_IP}};
		it2++;
		if (it2 != this->tempSegments.end() && it2->first.left() <= item.right())
			return {std::pair{BROADCAST_IP, BROADCAST_IP}};
	}
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->segments.insert_or_assign(item, std::pair{time + kDHCPTime * 1000LL, mac});
	long long int count = this->nowCount;
	this->nowCount += (1LL << 4);
	if (this->nowCount >= this->maxCount)
		this->nowCount = 0;
	return {IP(this->ip.intValue() + count),IP(0xffu,0xffu,0xffu,0xf0u)};
}

IP DHCPTable::directApply(const MAC& mac) {
	this->check();
	TableItem item(this->nowCount, 1);
	auto it = this->segments.upper_bound(item);
	if (it != this->segments.begin()) {
		it--;
		if (it->first.right() >= item.left())
			return BROADCAST_IP;
	}
	auto it2 = this->tempSegments.upper_bound(item);
	if (it2 != this->tempSegments.begin()) {
		it2--;
		if (it2->first.right() >= item.left())
			return BROADCAST_IP;
	}
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->segments.insert_or_assign(item, std::pair{time + kDHCPTime * 1000LL, mac });
	long long int count = this->nowCount++;
	if (this->nowCount >= this->maxCount)
		this->nowCount = 0;
	return IP(this->ip.intValue() + count);
}

bool DHCPTable::directApplySegment(const IP &ip, const IP &mask, const MAC &mac) {
	this->check();
	if ((mask & this->mask) != this->mask)
		return false;
	if ((ip & this->mask) != this->ip)
		return false;
	if (ip.intValue() < this->ip.intValue())
		return false;
	int zeros = mask.getRightZero();
	TableItem item(ip.intValue() - this->ip.intValue(), (1LL<<zeros) - (this->ip.intValue() - (this->ip & this->mask).intValue()) + 1);
	auto it = this->segments.upper_bound(item);
	if (it != this->segments.begin()) {
		it--;
		if (it->first.right() >= item.left())
			return false;
		it++;
		if (it != this->segments.end() && it->first.left() <= item.right())
			return false;
	}
	auto it2 = this->tempSegments.upper_bound(item);
	if (it2 != this->tempSegments.begin()) {
		it2--;
		if (it2->first.right() >= item.left())
			return false;
		it2++;
		if (it2 != this->tempSegments.end() && it2->first.left() <= item.right())
			return false;
	}
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->segments.insert_or_assign(item, std::pair{time + kDHCPTime * 1000LL, mac});
	return true;
}

bool DHCPTable::directApply(const IP &ip, const MAC &mac) {
	this->check();
	if ((ip & this->mask) != this->ip || ip.intValue() < this->ip.intValue())
		return false;
	TableItem item(ip.intValue() - this->ip.intValue(), 1);
	auto it = this->segments.upper_bound(item);
	if (it != this->segments.begin()) {
		it--;
		if (it->first.right() >= item.left())
			return false;
	}
	auto it2 = this->tempSegments.upper_bound(item);
	if (it2 != this->tempSegments.begin()) {
		it2--;
		if (it2->first.right() >= item.left())
			return false;
	}
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->segments.insert_or_assign(item, std::pair{time + kDHCPTime * 1000LL, mac});
	return this->dhcpID++;
}

bool DHCPTable::applyIt(const IP& ip, const IP& mask, const MAC& mac, int id) {
	int zeros = mask.getRightZero();
	TableItem item(ip.intValue() - this->ip.intValue(), (1LL<<zeros) - (this->ip.intValue() - (this->ip & this->mask).intValue()) + 1);
	if (this->tempSegments.find(item) == this->tempSegments.end())
		return false;
	auto it = this->tempSegments.at(item);
	if (it.second != id)
		return false;
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->segments.insert_or_assign(item, std::pair{time + kDHCPTime, mac});
	this->tempSegments.erase(item);
	return true;
}

bool DHCPTable::applyIt(const IP& ip, const MAC& mac, int id) {
	TableItem item(ip.intValue() - this->ip.intValue(), 1);
	if (this->tempSegments.find(item) == this->tempSegments.end())
		return false;
	auto it = this->tempSegments.at(item);
	if (it.second != id)
		return false;
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->segments.insert_or_assign(item, std::pair{time + kDHCPTime, mac});
	this->tempSegments.erase(item);
	return true;
}

bool DHCPTable::renewal(const IP& ip, const IP& mask, const MAC& mac) {
	int zeros = mask.getRightZero();
	TableItem item(ip.intValue() - this->ip.intValue(), (1LL<<zeros) - (this->ip.intValue() - (this->ip & this->mask).intValue()) + 1);
	auto it = this->segments.find(item);
	if (it == this->segments.end())
		return false;
	if (it->second.second != mac)
		return false;
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->segments.insert_or_assign(item, std::pair{time + kDHCPTime, mac});
	return true;
}

bool DHCPTable::renewal(const IP& ip, const MAC& mac) {
	TableItem item(ip.intValue() - this->ip.intValue(), 1);
	auto it = this->segments.find(item);
	if (it == this->segments.end())
		return false;
	if (it->second.second != mac)
		return false;
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	this->segments.insert_or_assign(item, std::pair{time + kDHCPTime, mac});
	return true;
}

DHCPTable::TableItem::TableItem(long long int start, long long int last) :start(start),last(last) {
}

bool DHCPTable::TableItem::operator<(const DHCPTable::TableItem &item) const {
	return this->left() < item.left();
}

long long int DHCPTable::TableItem::right() const {
	return this->start + this->last - 1;
}

long long int DHCPTable::TableItem::left() const {
	return this->start;
}
