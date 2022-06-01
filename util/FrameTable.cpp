//
// Created by 周蜀杰 on 2022/6/2.
//

#include "FrameTable.h"
#include "LinkLayer.h"

const unsigned char kFrameHeader = 0xf8;
const unsigned char kFrameFooter = 0xc6;
const unsigned char kFrameEscape = '\\';

const long long kFrameTime = 2LL * 60 * 1000 * 1000;

void FrameTable::add(Frame *frame) {
	this->check();
	auto time = std::chrono::system_clock::now().time_since_epoch().count() + kFrameTime;
	this->frameTable.insert_or_assign(frame->getStart(), std::pair{frame, time});
}

void FrameTable::check() {
	auto now = std::chrono::system_clock::now().time_since_epoch().count();
	for (auto it = this->frameTable.begin(); it != this->frameTable.end();)
		if (now - it->second.second > kFrameTime) {
			delete it->second.first;
			it = this->frameTable.erase(it);
		} else
			++it;
}

Frame *FrameTable::get(int start) {
	this->check();
	auto it = this->frameTable.find(start);
	if (it == this->frameTable.end())
		return nullptr;
	return it->second.first;
}

FrameTable::FrameTable(LinkLayer *layer) : layer(layer) {}

void put(std::vector<bool> *vector, unsigned char byte) {
	for (int i = 0; i < 8; ++i) {
		vector->push_back(byte & 0x80);
		byte <<= 1;
	}
}

unsigned char get(std::vector<bool> *vector, int index) {
	unsigned char byte = 0;
	for (int i = 0; i < 8; ++i) {
		byte <<= 1;
		byte |= vector->at(index + i);
	}
	return byte;
}

unsigned int CRC(const unsigned char* data, int length) {
	unsigned int crc = 0xffffffff;
	for (int i = 0; i < length; i++) {
		crc = crc ^ data[i];
		for (int j = 0; j < 8; j++) {
			if ((crc & 0x1) == 1) {
				crc = (crc >> 1) ^ 0xedb88320;
			} else {
				crc = crc >> 1;
			}
		}
	}
	return crc;
}

Block *FrameTable::readFrame(Block *block) {
	int size = block->getRemaining();
	auto *temp = new unsigned char[size];
	block->read(temp, size);
	std::vector<bool> bits;
	for (int i = 0; i < size; i++)
		put(&bits, temp[i]);
	delete[] temp;
	bool isStart = false;
	bool isEscape = false;
	auto* b = new Block();
	for (int i = 0; i + 8 <= bits.size(); i++) {
		unsigned char c = ::get(&bits, i);
		if (isEscape) {
			isEscape = false;
			b->write(c);
			i += 7;
		} else if (c == kFrameEscape && isStart) {
			isEscape = true;
			i += 7;
		} else if (c == kFrameHeader) {
			if (isStart)
				b->clear();
			isStart = true;
			b->write(c);
			i += 7;
		} else if (c == kFrameFooter) {
			b->write(c);
			if (isStart)
				break;
		} else if (isStart) {
			b->write(c);
			i += 7;
		}
	}
	block->flip();
	if (block->getRemaining() < 21) {
//		this->layer->send()
// todo send repacket
		return nullptr;
	}
	unsigned char header;
	block->read(&header, 1);
	if (header != kFrameHeader) {
		//todo re
		return nullptr;
	}
	int sequenceNumber = block->readInt();
	int index = block->readInt();
	int count = block->readInt();
	int length = block->readInt();
	int newLen = length % 8 == 0 ? length : length + 8 - length % 8;
	newLen = newLen / 8 * 13;
	if (block->getRemaining() < newLen) {
		//todo re
		return nullptr;
	}
	auto * content = new unsigned char[newLen];
	block->read(content, newLen);
	if (block->getRemaining() < 5) {
		//todo re
		return nullptr;
	}
	int crc = block->readInt();
	if (CRC(content, newLen) != crc) {
		//todo re
		return nullptr;
	}
	block->read(&header, 1);
	if (header != kFrameFooter) {
		//todo re
		delete[] content;
		return nullptr;
	}
	auto *frame = this->get(sequenceNumber);
	//todo
}
