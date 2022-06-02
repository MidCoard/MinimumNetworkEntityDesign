//
// Created by 周蜀杰 on 2022/5/27.
//

#include "Block.h"
#include "iostream"

Block::Block() {
	this->temp = {};
	this->remaining = 0;
	this->pos = 0;
	this->sendCount = 4;
}

void Block::write(unsigned char *data, int len) {
	this->temp.insert(this->temp.end(), data, data + len);
}

void Block::flip() {
	this->remaining = this->temp.size();
	this->pos = 0;
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
	std::vector<unsigned char> data = {this->temp.begin() + this->pos, this->temp.end()};
	this->pos = this->remaining;
	this->remaining = 0;
	return data;
}

unsigned char Block::readUnsignChar() {
	unsigned char data = this->temp[this->pos++];
	this->remaining--;
	return data;
}

MAC Block::readMAC() {
	if (this->remaining < 6)
		throw std::range_error("the rest of data is not enough to read MAC");
	MAC mac = MAC(this->temp[pos], this->temp[pos + 1], this->temp[pos + 2], this->temp[pos + 3], this->temp[pos + 4],
	              this->temp[pos + 5]);
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

void Block::writeMAC(const MAC &mac) {
	for (int i = 0; i < 6; i++)
		this->temp.push_back(mac.get(i));
}

void Block::write(const std::vector<unsigned char> &data) {
	for (unsigned char i: data)
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

void Block::writeIP(const IP &ip) {
	for (int i = 0; i < 4; i++)
		this->temp.push_back(ip.get(i));
}

Block::Block(Block *block) : Block() {
	this->writeBlock(block);
	this->flip();
}

void Block::writeHeader(Packet *packet) {
	this->temp.push_back(packet->getHeader());
}

Block::Block(int sendCount) : Block() {
	this->sendCount = sendCount;
}

int Block::getSendCount() const {
	return sendCount;
}

void Block::write(const unsigned char c) {
	this->temp.push_back(c);
}

Block *Block::copy() {
	auto *block = new Block(this->sendCount);
	block->pos = this->pos;
	block->remaining = this->remaining;
	for (unsigned char i : this->temp)
		block->temp.push_back(i);
	return block;
}

void Block::writeInt(int i) {
	this->temp.push_back(i >> 24);
	this->temp.push_back(i >> 16);
	this->temp.push_back(i >> 8);
	this->temp.push_back(i);
}

int Block::readInt() {
	if (this->remaining < 4)
		throw std::range_error("the rest of data is not enough to read int");
	int i = 0;
	i |= this->temp[this->pos++] << 24;
	i |= this->temp[this->pos++] << 16;
	i |= this->temp[this->pos++] << 8;
	i |= this->temp[this->pos++];
	this->remaining -= 4;
	return i;
}

long long int Block::readLong() {
	if (this->remaining < 8)
		throw std::range_error("the rest of data is not enough to read long");
	long long int i = 0;
	i |= (long long int) this->temp[this->pos++] << 56;
	i |= (long long int) this->temp[this->pos++] << 48;
	i |= (long long int) this->temp[this->pos++] << 40;
	i |= (long long int) this->temp[this->pos++] << 32;
	i |= (long long int) this->temp[this->pos++] << 24;
	i |= (long long int) this->temp[this->pos++] << 16;
	i |= (long long int) this->temp[this->pos++] << 8;
	i |= (long long int) this->temp[this->pos++];
	this->remaining -= 8;
	return i;
}

void Block::writeLong(long long int i) {
	this->temp.push_back(i >> 56);
	this->temp.push_back(i >> 48);
	this->temp.push_back(i >> 40);
	this->temp.push_back(i >> 32);
	this->temp.push_back(i >> 24);
	this->temp.push_back(i >> 16);
	this->temp.push_back(i >> 8);
	this->temp.push_back(i);
}

long long Block::size() const {
	return this->remaining + this->pos;
}

// used for debug
unsigned char Block::view(int pos) {
	return this->temp[pos];
}

// used for debug
MAC Block::viewMAC(int pos) {
	MAC mac = MAC(this->temp[pos], this->temp[pos + 1], this->temp[pos + 2], this->temp[pos + 3], this->temp[pos + 4],
	              this->temp[pos + 5]);
	return mac;
}

// used for debug
IP Block::viewIP(int pos) {
	IP ip = IP(this->temp[pos], this->temp[pos + 1], this->temp[pos + 2], this->temp[pos + 3]);
	return ip;
}

void Block::clear() {
	this->temp.clear();
	this->pos = 0;
	this->remaining = 0;
}

void Block::print() {
	for (unsigned char i : this->temp)
		std::cout << "0x" << util::toHex(i) << ",";
	std::cout << std::endl;
}

