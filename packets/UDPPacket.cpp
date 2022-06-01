#include "UDPPacket.h"

#include <utility>

const int kPacketSize = 1024;

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
	int blockSize = pos == this->size - 1 ? this->data.size() - pos * kPacketSize : kPacketSize;
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
	this->size = this->data.size() % kPacketSize == 0 ? this->data.size() / kPacketSize : this->data.size() / kPacketSize + 1;
	this->offset = 0;
	this->pos = 0;
}
