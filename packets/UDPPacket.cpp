#include "UDPPacket.h"

#include <utility>

unsigned char UDPPacket::getHeader() {
	return 0x60;
}

Block *UDPPacket::createBlock() {
	// make each block size is 1024 bytes
	// inside it, 4 for destination IP, 1 for header, 4 for block index, 4 for blocks size, 4 for udp count, 2 for block size, 1005 for block content
	this->size = this->data.size() % 1011 == 0 ? this->data.size() / 1005 : this->data.size() / 1005 + 1;
	this->blocks = new Block*[this->size];
	int offset = 0;
	for (int i = 0; i < this->size; i++) {
		this->blocks[i] = new Block();
		this->blocks[i]->writeIP(this->ip);
		this->blocks[i]->writeHeader(this);
		this->blocks[i]->writeInt(i);
		this->blocks[i]->writeInt(this->size);
		this->blocks[i]->writeInt(count);
		int blockSize = i == this->size - 1 ? this->data.size() - i * 1005 : 1005;
		this->blocks[i]->writeInt(blockSize);
		for (int j = 0; j < blockSize; j++)
			this->blocks[i]->write(this->data[offset + j]);
		offset += blockSize;
	}
	return *this->blocks;
}

void UDPPacket::write(const std::vector<unsigned char>& data) {
	for (auto& byte : data)
		this->data.push_back(byte);
}

UDPPacket::UDPPacket(IP ip, int count) :ip(std::move(ip)),count(count) {}

int UDPPacket::getSize() const {
	return this->size;
}
