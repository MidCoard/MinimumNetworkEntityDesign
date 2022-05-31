//
// Created by 周蜀杰 on 2022/5/29.
//

#include "UDPTable.h"
#include "AppLayer.h"

void UDPTable::add(Block * block) {
	mutex.lock();
	int index = block->readInt();
	int size = block->readInt();
	int count = block->readInt();
	int wholeLength = block->readInt();
	if (this->table.find(count) == this->table.end())
		return;
	std::vector<unsigned char> data = block->read();
	this->table[count].insert_or_assign(index,data );
	if (this->table[count].size() == size) {
		auto * buffer = new unsigned char[wholeLength];
		int pos = 0;
		for (int i = 0; i < size; i++) {
			std::vector<unsigned char> temp = this->table[count][i];
			for (unsigned char j : temp)
				buffer[pos++] = j;
		}
		this->table.erase(count);
		this->layer->handleUDPData(buffer,wholeLength);
		delete[] buffer;
	}
	mutex.unlock();
}

int UDPTable::tryAllocate() {
	mutex.lock();
	this->table.insert_or_assign(this->count, std::map<int,std::vector<unsigned char>>());
	int result = this->count++;
	mutex.unlock();
	return result;
}

UDPTable::UDPTable(AppLayer *layer) :layer(layer) {}
