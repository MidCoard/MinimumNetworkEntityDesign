//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPTable.h"
#include <limits>
#include <utility>

const int kApplyCount = 20;
const long long int kDHCPTime = 2LL * 10 * 1000 * 1000;
//const long long int kDHCPTime = 2LL * 60 * 60 * 1000 * 1000;

std::pair<IP,long long int> DHCPTable::apply() {
	int count = 0;
	auto a = apply0();
	while (a.second == -1 && count < kApplyCount) {
		a = apply0();
		count++;
	}
	return a;
}

// use sequence to avoid the same ip segment
std::pair<IP,long long int> DHCPTable::apply0() {
	this->check();
	TableItem item(this->nowCount++, 1);
	if (this->nowCount >= this->maxCount)
		this->nowCount = 0;
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
	return {IP(this->ip.intValue() + item.left()), this->dhcpID++};
}

unsigned long long DHCPTable::apply(const IP& ip) {
	this->check();
	if ((ip & this->mask) != (this->ip & this->mask) || ip.intValue() < this->ip.intValue())
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
	if ((ip & this->mask) != (this->ip & this->mask))
		return -1;
	if (ip.intValue() < this->ip.intValue())
		return -1;
	int zeros = mask.getRightZero();
	TableItem item(ip.intValue() - this->ip.intValue(), (1LL<<zeros) - (ip.intValue() - (ip & mask).intValue()));
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

std::pair<std::pair<IP,IP>,long long int> DHCPTable::tryApply(const IP &ip, const IP &mask) {
	this->check();
	if ((mask & this->mask) != this->mask)
		return {std::pair<IP,IP>{BROADCAST_IP, BROADCAST_IP}, -1};
	if ((ip & this->mask) != (this->ip & this->mask))
		return {std::pair<IP,IP>{BROADCAST_IP, BROADCAST_IP}, -1};
	int zeros = mask.getRightZero();
	TableItem item(ip.intValue() < this->ip.intValue() ? 0 : ip.intValue() - this->ip.intValue(), (1LL<<zeros) - (ip.intValue() - (ip & mask).intValue()));
	std::pair<long long int, long long int> a = {item.left() , item.right()};
	std::pair<long long int, long long int> b = {item.left(), item.right()};
	auto it = this->segments.upper_bound(item);
	if (it != this->segments.begin()) {
		it--;
		if (it->first.right() >= item.left())
			a.first = it->first.right() + 1;
		it++;
		if (it != this->segments.end() && it->first.left() <= item.right())
			a.second = it->first.left() - 1;
	}
	auto it2 = this->tempSegments.upper_bound(item);
	if (it2 != this->tempSegments.begin()) {
		it2--;
		if (it2->first.right() >= item.left())
			b.first = it2->first.right() + 1;
		it2++;
		if (it2 != this->tempSegments.end() && it2->first.left() <= item.right())
			b.second = it2->first.left() - 1;
	}
	if (a.first > a.second || b.first > b.second)
		return {std::pair<IP,IP>{BROADCAST_IP, BROADCAST_IP}, -1};
	// get the two ranges a and b intersection
	long long int min = std::max(a.first, b.first);
	long long int max = std::min(a.second, b.second);
	if (min > max)
		return {std::pair<IP,IP>{BROADCAST_IP, BROADCAST_IP}, -1};
	else  {
		IP ip1 = IP(this->ip.intValue() + min);
		long long int length = max - min + 1;
		long long int generate = length + min;
		// get length highest bit
		int i = 0;
		while (generate >> i)
			i++;
		IP retMask = IP(~((1LL<<(i-1)) - 1));
		TableItem newItem = TableItem(min, length);
		auto time = std::chrono::system_clock::now().time_since_epoch().count();
		this->tempSegments.insert_or_assign(newItem, std::pair{time + 2LL * 60 * 1000 * 1000, dhcpID});
		return {std::pair<IP,IP>{ip1,retMask},this->dhcpID++};
	}
}

std::pair<std::pair<IP,IP>,long long int> DHCPTable::applySegment0() {
	this->check();
	int zeros = this->mask.getRightZero();
	if (zeros < 4)
		return {std::pair{BROADCAST_IP, BROADCAST_IP}, -1};
	long long int rest = (this->nowCount + this->ip.intValue()) % 16;
	TableItem item(this->nowCount + (16 - rest), 1LL << 4);
	this->nowCount = (this->nowCount + (16 - rest)) + 16;
	if (this->nowCount >= this->maxCount)
		this->nowCount = 0;
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
	return {{IP(this->ip.intValue() + item.left()),IP(0xffu,0xffu,0xffu,0xf0u)},this->dhcpID++};
}

std::pair<std::pair<IP,IP>,long long int> DHCPTable::applySegment() {
	int count = 0;
	auto a = applySegment0();
	while (a.second == -1 && count < kApplyCount) {
		a = applySegment0();
		count++;
	}
	return a;
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

std::pair<IP, IP> DHCPTable::directApplySegment0(const MAC& mac) {
	this->check();
	int zeros = this->mask.getRightZero();
	if (zeros < 4)
		return {std::pair{BROADCAST_IP, BROADCAST_IP}};
	long long int rest = (this->nowCount + this->ip.intValue()) % 16;
	TableItem item(this->nowCount + (16 - rest), 1LL << 4);
	this->nowCount = (this->nowCount + (16 - rest)) + 16;
	if (this->nowCount >= this->maxCount)
		this->nowCount = 0;
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

	return {IP(this->ip.intValue() + item.left()),IP(0xffu,0xffu,0xffu,0xf0u)};
}

std::pair<IP, IP> DHCPTable::directApplySegment(const MAC& mac) {
	int count = 0;
	auto a = directApplySegment0(mac);
	while (a.first.isBroadcast() && count < kApplyCount) {
		a = directApplySegment0(mac);
		count++;
	}
	return a;
}

IP DHCPTable::directApply0(const MAC& mac) {
	this->check();
	TableItem item(this->nowCount++, 1);
	if (this->nowCount >= this->maxCount)
		this->nowCount = 0;
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
	return IP(this->ip.intValue() + item.left());
}

IP DHCPTable::directApply(const MAC& mac) {
	int count = 0;
	auto a = directApply0(mac);
	while (a.isBroadcast() && count < kApplyCount) {
		a = directApply0(mac);
		count++;
	}
	return a;
}

bool DHCPTable::directApplySegment(const IP &ip, const IP &mask, const MAC &mac) {
	this->check();
	if ((mask & this->mask) != this->mask)
		return false;
	if ((ip & this->mask) != (this->ip & this->mask))
		return false;
	if (ip.intValue() < this->ip.intValue())
		return false;
	int zeros = mask.getRightZero();
	TableItem item(ip.intValue() - this->ip.intValue(), (1LL<<zeros) - (ip.intValue() - (ip & mask).intValue()));
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
	if ((ip & this->mask) != (this->ip & this->mask) || ip.intValue() < this->ip.intValue())
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
	return true;
}

bool DHCPTable::applyIt(IP* ip, IP* mask, const MAC& mac, int id) {
	if (id == -1) {
		auto part = tryApply(*ip, *mask);
		*ip = part.first.first;
		*mask = part.first.second;
		id = part.second;
		if (id == -1)
			return false;
	}
	int zeros = mask->getRightZero();
	TableItem item(ip->intValue() - this->ip.intValue(), (1LL<<zeros) - (ip->intValue() - (*ip & *mask).intValue()));
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
	if (id == -1) {
		id = apply(ip);
		if (id == -1)
			return false;
	}
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
	TableItem item(ip.intValue() - this->ip.intValue(), (1LL<<zeros) - (ip.intValue() - (ip & mask).intValue()));
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

void DHCPTable::print() {
	std::cout << "IP: " << this->ip.str() << " Mask: " << this->mask.str() << std::endl;
	for (auto & segment : this->segments) {
		std::cout << "Segment: " << segment.first.left() << " " << segment.first.right() << " " << segment.second.first << " " << segment.second.second.str() << std::endl;
	}
	for (auto & tempSegment : this->tempSegments) {
		std::cout << "TempSegment: " << tempSegment.first.left() << " " << tempSegment.first.right() << " " << tempSegment.second.first << " " << tempSegment.second.second << std::endl;
	}
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
