//
// Created by 周蜀杰 on 2022/5/27.
//

#include "Block.h"

Block::Block() {
	this->data = nullptr;
	this->temp = {};
	this->remaining = 0;
	this->pos = 0;
}

void Block::write(unsigned char *data, int len) {
	this->temp.insert(this->temp.end(), data, data + len);
}

void Block::flip() {
	this->data = new unsigned char[this->temp.size()];
	for (int i = 0; i < this->temp.size(); i++)
		this->data[i] = this->temp.at(i);
	this->remaining = this->temp.size();
	this->temp.clear();
}

Block::~Block() {
	delete[] this->data;
}

int Block::getRemaining() const {
	return this->remaining;
}

int Block::read(unsigned char *data, int len) {
	if (len > this->remaining)
		len = this->remaining;
	for (int i = 0; i < len; i++)
		data[i] = this->data[this->pos++];
	this->remaining -= len;
	return len;
}

std::vector<unsigned char> Block::getData() {
	this->pos = this->remaining;
	this->remaining = 0;
	return {this->data, this->data + this->remaining};
}
