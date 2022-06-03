#include "UDPPacket.h"

#include <utility>

const int kPacketSize = 1024;

unsigned char UDPPacket::getHeader() {
	return 0x60;
}

Block *UDPPacket::createBlock(int pos) {
	if (pos >= this->size)
		return nullptr;
	auto* block = new Block();
	block->writeIP(this->ip);
	block->write(0);
	block->writeHeader(this);
	block->writeIP(this->source);
	block->writeInt(pos); // index
	block->writeInt(count); // id
	int blockSize = pos == this->size - 1 ? this->data.size() - pos * kPacketSize : kPacketSize;
	for (int j = 0; j < blockSize; j++)
		block->write(this->data[pos * kPacketSize + j]);
	block->flip();
	return block;
}

UDPPacket::UDPPacket(IP ip,IP source, unsigned char* data,int len) :ip(std::move(ip)),source(std::move(source)) {
	for (int i = 0; i < len; i++)
		this->data.push_back(data[i]);
	this->size = this->data.size() % kPacketSize == 0 ? this->data.size() / kPacketSize : this->data.size() / kPacketSize + 1;
}

int UDPPacket::getSize() const {
	return this->size;
}

void UDPPacket::init(int count) {
	this->count = count;
}

Block *UDPPacket::createBlock() {
	return nullptr;
}
