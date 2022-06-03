//
// Created by 周蜀杰 on 2022/5/29.
//

#include "AppLayer.h"
#include "UDPPreTable.h"
#include "UDPPacket.h"
#include "UDPACKPacket.h"

const int kPacketTime = 2LL * 60 * 1000 * 1000;

std::pair<int, int> UDPPreTable::tryAllocate(const IP& ip,const IP& source, unsigned char *data, int len) {
	mutex.lock();
	this->check();
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	auto* packet = new UDPPacket(ip,source,data,len);
	this->table.insert_or_assign(this->count, std::pair{packet,std::pair{5,time + kPacketTime}});
	std::pair<int,int> ret = {this->count++, packet->getSize()};
	mutex.unlock();
	return ret;
}

void UDPPreTable::send(const IP &ip, const IP &source, int count, int target) {
	mutex.lock();
	this->check();
	auto it = this->table.find(count);
	if (it == this->table.end()) {
		mutex.unlock();
		return;
	}
	it->second.second.first = 0;
	auto* packet = it->second.first;
	packet->init(target);
	int size = packet->getSize();
	for (int i = 0; i < size; i++) {
		this->layer->send(packet->createBlock(i));
	}
	auto* ack = new UDPACKPacket(ip,source, target);
	this->layer->send( ack->createBlock());
	delete ack;
	mutex.unlock();
}

void UDPPreTable::check() {
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	for (auto it = this->table.begin(); it != this->table.end();) {
		if (time > it->second.second.second) {
			delete it->second.first;
			it = this->table.erase(it);
		} else {
			it++;
		}
	}
}

UDPPreTable::UDPPreTable(AppLayer *layer) {
	this->layer = layer;
}

bool UDPPreTable::requestResendPre(const IP &ip, int count) {
	mutex.lock();
	this->check();
	auto it = this->table.find(count);
	if (it == this->table.end()) {
		mutex.unlock();
		return false;
	}
	if (it->second.second.first <= 0) {
		mutex.unlock();
		return false;
	} else {
		it->second.second.first--;
		mutex.unlock();
		return true;
	}
}

void UDPPreTable::resend(const IP& ip,const IP& source, int count, const std::vector<int>& ids) {
	mutex.lock();
	this->check();
	auto it = this->table.find(count);
	if (it == this->table.end()) {
		mutex.unlock();
		return;
	}
	auto* packet = it->second.first;
	for (auto id : ids) {
		this->layer->send(packet->createBlock(id));
	}
	auto* ack = new UDPACKPacket(ip,source, packet->getCount());
	this->layer->send( ack->createBlock());
	delete ack;
	mutex.unlock();
}
