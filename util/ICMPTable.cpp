//
// Created by 周蜀杰 on 2022/5/29.
//

#include "ICMPTable.h"

void ICMPTable::remove(const IP& ip,const IP& query) {
	if (this->map.find({ip,query}) != this->map.end())
		this->map.erase({ip,query});
}

void ICMPTable::add(const IP& ip, const IP& query) {
	this->map.insert_or_assign(std::pair{ip,query},2);
}

void ICMPTable::update(const IP& ip, const IP& query, bool flag) {
	// check if ip and query are in the table
	if (this->map.find({ip,query}) != this->map.end())
		this->map.at({ip,query}) = flag;
}

int ICMPTable::lookup(const IP &ip, const IP &query) {
	if (this->map.find({ip,query}) != this->map.end())
		return this->map.at({ip,query});
	else
		return -1;
}
