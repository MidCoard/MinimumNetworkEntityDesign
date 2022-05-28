//
// Created by 周蜀杰 on 2022/5/27.
//

#include "Block.h"

Block::Block() {
	this->temp = {};
	this->remaining = 0;
	this->pos = 0;
}

void Block::write(unsigned char *data, int len) {
	this->temp.insert(this->temp.end(), data, data + len);
}

void Block::flip() {
	this->remaining = this->temp.size();
}

int Block::getRemaining() const {
	return this->remaining;
}

int Block::read(unsigned char *data, int len) {
	if (len > this->remaining)
		len = this->remaining;
	for (int i = 0; i < len; i++)
		data[i] = this->temp[this->pos++];
	this->remaining -= len;
	return len;
}

std::vector<unsigned char> Block::read() {
	this->pos = this->remaining;
	this->remaining = 0;
	return {this->temp.begin(), this->temp.end()};
}

MAC Block::readMAC() {
	if (this->remaining < 6)
		throw std::range_error("the rest of data is not enough to read MAC");
	MAC mac = MAC(this->temp[pos], this->temp[pos + 1], this->temp[pos + 2], this->temp[pos + 3], this->temp[pos + 4], this->temp[pos + 5]);
	this->pos += 6;
	this->remaining -= 6;
	return mac;
}

void Block::writeBlock(Block *block) {
	for (int i = 0; i < block->getRemaining(); i++)
		this->temp.push_back(block->temp[block->pos + i]);
	block->pos += block->getRemaining();
	block->remaining = 0;
}

void Block::writeMAC(const MAC& mac) {
	for (int i = 0; i < 6; i++)
		this->temp.push_back(mac.get(i));
}

void Block::write(const std::vector<unsigned char>& data) {
	for (unsigned char i : data)
		this->temp.push_back(i);
}

IP Block::readIP() {
	if (this->remaining < 4)
		throw std::range_error("the rest of data is not enough to read IP");
	IP ip = IP(this->temp[pos], this->temp[pos + 1], this->temp[pos + 2], this->temp[pos + 3]);
	this->pos += 4;
	this->remaining -= 4;
	return ip;
}

void Block::writeIP(const IP& ip) {
	for (int i = 0; i < 4; i++)
		this->temp.push_back(ip.get(i));
}

Block::Block(Block *block) :Block() {
	this->writeBlock(block);
	this->flip();
}


