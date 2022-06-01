//
// Created by 周蜀杰 on 2022/5/29.
//

#include "AppLayer.h"
#include "UDPPreTable.h"
#include "UDPPacket.h"

int UDPPreTable::tryAllocate(unsigned char *data, int len) {
	mutex.lock();
	std::vector<unsigned char> v(data, data + len);
	this->table.insert_or_assign(this->count, v);
	int ret = this->count++;
	mutex.unlock();
	return ret;
}

void UDPPreTable::send(int count, int target, const IP& ip) {
	mutex.lock();
	if (this->table.find(count) == this->table.end()) {
		mutex.unlock();
		return;
	}
	std::vector<unsigned char> v = this->table[count];
	auto* packet = new UDPPacket(ip, target);
	packet->write(v);
	packet->init();
	int size = packet->getSize();
	for (int i = 0; i < size; i++)
		this->layer->send(packet->createBlock());
	delete packet;
	this->table.erase(count);
	mutex.unlock();
}

UDPPreTable::UDPPreTable(AppLayer *layer) {
	this->layer = layer;
}
