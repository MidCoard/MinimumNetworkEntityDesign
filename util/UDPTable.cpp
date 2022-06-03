//
// Created by 周蜀杰 on 2022/5/29.
//

#include "UDPTable.h"
#include "AppLayer.h"

int UDPTable::add(Block * block,const IP& ip,int count, int index ) {
	mutex.lock();
	this->check();
	auto it = this->idTable.find(std::pair{ip,count});
	if (this->table.find(count) == this->table.end() || it == this->idTable.end()) {
		mutex.unlock();
		return 0;
	}
	std::vector<unsigned char> data = block->read();
	this->table[count].insert_or_assign(index,data );
	mutex.unlock();
	return data.size();
}

int UDPTable::tryAllocate(const IP& ip, int count, int size, int wholeLength) {
	mutex.lock();
	this->check();
	auto item = std::pair{ip,count};
	auto it = this->idTable.find(item);
	if (it != this->idTable.end()) {
		mutex.unlock();
		return it->second.first;
	}
	int result = this->count++;
	this->idTable.insert_or_assign(item,std::pair{result, std::pair{size,wholeLength}});
	this->table.insert_or_assign(result, std::map<int,std::vector<unsigned char>>());
	mutex.unlock();
	return result;
}

UDPTable::UDPTable(AppLayer *layer) :layer(layer) {}

void UDPTable::ack(const IP& ip,int count) {
	mutex.lock();
	this->check();
	auto it = this->idTable.find(std::pair{ip,count});
	if (this->table.find(count) == this->table.end() || it == this->idTable.end()) {
		mutex.unlock();
		return;
	}
	int size = it->second.second.first;
	int wholeLength = it->second.second.second;
	if (this->table[count].size() == size) {
		auto * buffer = new unsigned char[wholeLength];
		int pos = 0;
		for (int i = 0; i < size; i++) {
			std::vector<unsigned char> temp = this->table[count][i];
			for (unsigned char j : temp)
				buffer[pos++] = j;
		}
		this->table.erase(count);
		this->idTable.erase(it);
		this->layer->handleUDPData(buffer,wholeLength);
		delete[] buffer;
	} else {
		std::vector<int> ids;
		for (int i = 0; i< size;i ++)
			if (this->table[count].find(i) == this->table[count].end())
				ids.push_back(i);
		auto * packet = new UDPRequestPacket();
	}
	mutex.unlock();
}

void UDPTable::check() {

}
