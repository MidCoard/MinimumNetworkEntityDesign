//
// Created by 周蜀杰 on 2022/5/29.
//

#include "AppLayer.h"
#include "UDPPreTable.h"
#include "UDPPacket.h"

int UDPPreTable::tryAllocate(unsigned char *data, int len) {
	std::vector<unsigned char> v(data, data + len);
	this->table.insert_or_assign(this->count, v);
	return this->count++;
}

void UDPPreTable::send(int count, int target, const IP& ip) {
	if (this->table.find(count) == this->table.end())
		return;
	std::vector<unsigned char> v = this->table[count];
	auto* packet = new UDPPacket(ip, target);
	packet->write(v);
	Block * begin = packet->createBlock();
	int size = packet->getSize();
	delete packet;
	for (int i = 0; i < size; i++)
		this->layer->send(begin + i);
	this->table.erase(count);
}

UDPPreTable::UDPPreTable(AppLayer *layer) {
	this->layer = layer;
}
