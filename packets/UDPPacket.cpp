#include "UDPPacket.h"

#include <utility>

unsigned char UDPPacket::getHeader() {
	return 0x60;
}

Block *UDPPacket::createBlock() {
	auto* block = new Block();
	block->writeIP(this->ip);
	block->write(0);
	block->writeHeader(this);
	block->writeInt(pos);
	block->writeInt(this->size);
	block->writeInt(count);
	int blockSize = pos == this->size - 1 ? this->data.size() - pos * 1003 : 1003;
	block->writeInt(this->data.size());
	for (int j = 0; j < blockSize; j++)
		block->write(this->data[offset + j]);
	block->flip();
	offset += blockSize;
	pos++;
	return block;
}

void UDPPacket::write(const std::vector<unsigned char>& data) {
	for (auto& byte : data)
		this->data.push_back(byte);
}

UDPPacket::UDPPacket(IP ip, int count) :ip(std::move(ip)),count(count) {

}

int UDPPacket::getSize() const {
	return this->size;
}

void UDPPacket::init() {
	// make each block size is 1024 bytes
	// inside it, 4 for destination IP,1 for applayer protocol, 1 for header, 4 for block index, 4 for blocks size, 4 for udp count, 4 for whole size, 1003 for block content
	this->size = this->data.size() % 1003 == 0 ? this->data.size() / 1003 : this->data.size() / 1003 + 1;
	this->offset = 0;
	this->pos = 0;
}
