//
// Created by 周蜀杰 on 2022/6/2.
//

#include "FrameTable.h"
#include "LinkLayer.h"

const unsigned char kFrameHeader = 0xf8;
const unsigned char kFrameFooter = 0xc6;
const unsigned char kFrameEscape = '\\';
const int kFramePacketSize = 800; // byte

const long long kFrameTime = 2LL * 60 * 1000 * 1000;

void FrameTable::add(Frame *frame) {
	this->check();
	auto time = std::chrono::system_clock::now().time_since_epoch().count() + kFrameTime;
	this->frameTable.insert_or_assign(frame->getStart(), std::pair{frame, time});
}

void FrameTable::check() {
	auto now = std::chrono::system_clock::now().time_since_epoch().count();
	for (auto it = this->frameTable.begin(); it != this->frameTable.end();)
		if (now > it->second.second) {
			delete it->second.first;
			it = this->frameTable.erase(it);
		} else
			++it;
	for (auto it = this->table.begin(); it != this->table.end();)
		if (now > it->second.first)
			it = this->table.erase(it);
		else
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
	if (block->getRemaining() < 21)
		return nullptr;
	unsigned char header;
	block->read(&header, 1);
	if (header != kFrameHeader)
		return nullptr;
	int sequence = block->readInt();// sequence
	int index = block->readInt();// index
	int count = block->readInt();// count
	int length = block->readInt();
	int newLen = length % 8 == 0 ? length : length + 8 - length % 8;
	newLen = newLen / 8 * 13;
	if (block->getRemaining() < newLen)
		return nullptr;
	auto * content = new unsigned char[newLen];
	block->read(content, newLen);
	std::bitset<kFramePacketSize * 13> newbits;
	for (int i = 0;i<newLen;i++) {
		newbits.set(i * 8, content[i] & 0x80);
		newbits.set(i * 8 + 1, content[i] & 0x40);
		newbits.set(i * 8 + 2, content[i] & 0x20);
		newbits.set(i * 8 + 3, content[i] & 0x10);
		newbits.set(i * 8 + 4, content[i] & 0x08);
		newbits.set(i * 8 + 5, content[i] & 0x04);
		newbits.set(i * 8 + 6, content[i] & 0x02);
		newbits.set(i * 8 + 7, content[i] & 0x01);
	}
	unsigned char t[13];
	for (int i = 0; i < newLen * 8; i+=13) {
		bool q1 = newbits.test(i);
		bool q2 = newbits.test(i + 1);
		bool a9 = newbits.test(i + 2);
		bool q3 = newbits.test(i + 3);
		bool a8 = newbits.test(i + 4);
		bool a7 = newbits.test(i + 5);
		bool a6 = newbits.test(i + 6);
		bool q4 = newbits.test(i + 7);
		bool a5 = newbits.test(i + 8);
		bool a4 = newbits.test(i + 9);
		bool a3 = newbits.test(i + 10);
		bool a2 = newbits.test(i + 11);
		bool a1 = newbits.test(i + 12);
		bool b1 = q1 ^ a9 ^ a8 ^ a6 ^ a5 ^ a3 ^ a1;
		bool b2 = q2 ^ a9 ^ a7 ^ a6 ^ a4 ^ a3;
		bool b3 = q3 ^ a8 ^ a7 ^ a6 ^ a2 ^ a1;
		bool b4 = q4 ^ a5 ^ a4 ^ a3 ^ a2 ^ a1;
		unsigned char status = b1 | (b2 << 1) | (b3 << 2) | (b4 << 3);
		t[0] = q1;
		t[1] = q2;
		t[2] = a9;
		t[3] = q3;
		t[4] = a8;
		t[5] = a7;
		t[6] = a6;
		t[7] = q4;
		t[8] = a5;
		t[9] = a4;
		t[10] = a3;
		t[11] = a2;
		t[12] = a1;
		t[status-1] = !t[status-1];
		if (t[2] != (a1 ^ a2 ^ a3 ^ a4 ^ a5 ^ a6 ^ a7 ^ a8)) {
			delete[] content;
			return nullptr;
		}
		unsigned char c = (a1 << 7) | (a2 << 6) | (a3 << 5) | (a4 << 4) | (a5 << 3) | (a6 << 2) | (a7 << 1) | a8;
		content[i / 13] = c;
	}
	if (block->getRemaining() < 5) {
		delete[] content;
		return nullptr;
	}
	newLen = newLen / 13 * 8;
	int crc = block->readInt();
	if (util::CRC(content, newLen) != crc) {
		delete[] content;
		return nullptr;
	}
	block->read(&header, 1);
	if (header != kFrameFooter) {
		delete[] content;
		return nullptr;
	}
	auto *ret = this->write(sequence, index, count, content, newLen);
	delete[] content;
	return ret;
}

Block * FrameTable::write(int sequence, int index, int count, unsigned char *buffer, int len) {
	auto time = std::chrono::system_clock::now().time_since_epoch().count() + kFrameTime;
	if (this->table.find(sequence) == this->table.end())
		this->table.insert_or_assign(sequence, std::make_pair(time, std::map<int,std::vector<unsigned char>>()));
	std::vector<unsigned char> vector(buffer, buffer + len);
	auto& pair = this->table[sequence];
	auto& map = pair.second;
	map.insert_or_assign(index, vector);
	if (map.size() == count) {
		auto* block = new Block();
		for (auto& p : map)
			block->write(p.second.data(), p.second.size());
		block->flip();
		return block;
	}
	return nullptr;
}
