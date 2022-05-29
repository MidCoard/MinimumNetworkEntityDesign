//
// Created by 周蜀杰 on 2022/5/29.
//

#include "ICMPTable.h"

void ICMPTable::remove(const IP& ip,const IP& query) {
	this->mtx.lock();
	if (this->map.find({ip,query}) != this->map.end())
		this->map.erase({ip,query});
	this->mtx.unlock();
}

void ICMPTable::add(const IP& ip, const IP& query) {
	this->mtx.lock();
	this->map.insert_or_assign(std::pair{ip,query},2);
	this->mtx.unlock();
}

void ICMPTable::update(const IP& ip, const IP& query, bool flag) {
	// check if ip and query are in the table
	this->mtx.lock();
	if (this->map.find({ip,query}) != this->map.end())
		this->map.at({ip,query}) = flag;
	this->mtx.unlock();
}

int ICMPTable::lookup(const IP &ip, const IP &query) {
	int ret;
	this->mtx.lock();
	if (this->map.find({ip,query}) != this->map.end())
		ret = this->map.at({ip,query});
	else
		ret = -1;
	this->mtx.unlock();
	return ret;
}
