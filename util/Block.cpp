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

std::vector<unsigned char> Block::getData() {
	this->pos = this->remaining;
	this->remaining = 0;
	return {this->temp.begin(), this->temp.end()};
}
