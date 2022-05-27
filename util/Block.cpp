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
	this->temp->insert(this->temp->end(), data, data + len);
}

void Block::flip() {
	this->data = new unsigned char[this->temp->size()];
	for (int i = 0; i < this->temp->size(); i++)
		this->data[i] = this->temp->at(i);
	this->remaining = this->temp->size();
	delete this->temp;
}

Block::~Block() {
	delete[] this->data;
}

int Block::getRemaining() const {
	return this->remaining;
}

void Block::read(unsigned char *data, int len) {
	for (int i = 0; i < len; i++)
		data[i] = this->data[this->pos++];
	this->remaining -= len;
}

std::vector<unsigned char> Block::getData() {
	return {this->data, this->data + this->remaining};
}
