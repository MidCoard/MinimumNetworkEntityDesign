//
// Created by 周蜀杰 on 2022/6/2.
//

#include "FrameTable.h"
#include "chrono"

const unsigned char kFrameHeader = 0xf8;
const unsigned char kFrameFooter = 0xc6;
const unsigned char kFrameEscape = '\\';
const long long kFrameTime = 2LL * 60 * 1000 * 1000;

void FrameTable::check() {
	auto now = util::getNowTime();
	for (auto it = this->table.begin(); it != this->table.end();)
		if (now > it->second.first)
			it = this->table.erase(it);
		else
			++it;
}

Block *FrameTable::readFrame(Block *block) {
	std::vector<bool> bits;
	while (block->getRemaining())
		util::put(&bits, block->readUnsignChar());
	bool isStart = false;
	bool isEscape = false;
	auto *b = new Block();
	for (int i = 0; i + 8 <= bits.size(); i++) {
		unsigned char c = util::get(&bits, i);
		if (isEscape) {
			isEscape = false;
			if (isStart) {
				b->write(c);
			}
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
			i += 7;
		} else if (isStart) {
			b->write(c);
			i += 7;
		}
	}
	b->flip();
	if (b->getRemaining() < 17) {
		delete b;
		return nullptr;
	}
	unsigned char header;
	b->read(&header, 1);
	if (header != kFrameHeader) {
		delete b;
		return nullptr;
	}
	int sequence = b->readInt();// sequence
	int index = b->readInt();// index
	int count = b->readInt();// count
	int wholeLength = b->readInt();
	if (b->getRemaining() < 5 || (b->getRemaining() - 5) % 13 != 0) {
		delete b;
		return nullptr;
	}
	int rest = b->getRemaining();
	auto *content = new unsigned char[rest - 5];
	b->read(content, rest - 5);
	unsigned int crc = b->readInt();
	b->read(&header, 1);
	if (header != kFrameFooter) {
		delete[] content;
		delete b;
		return nullptr;
	}
	delete b;
	std::vector<bool> newbits;
	std::vector<bool> newbits2;
	for (int i = 0; i < rest - 5; i++)
		util::put(&newbits, content[i]);
	bool temp[13];
	bool flag = false;
	for (int i = 0; i < newbits.size(); i += 13) {
		temp[0] = newbits[i];
		temp[1] = newbits[i + 1];
		temp[2] = newbits[i + 2];
		temp[3] = newbits[i + 3];
		temp[4] = newbits[i + 4];
		temp[5] = newbits[i + 5];
		temp[6] = newbits[i + 6];
		temp[7] = newbits[i + 7];
		temp[8] = newbits[i + 8];
		temp[9] = newbits[i + 9];
		temp[10] = newbits[i + 10];
		temp[11] = newbits[i + 11];
		temp[12] = newbits[i + 12];
		bool b1 = temp[0] ^ temp[2] ^ temp[4] ^ temp[6] ^ temp[8] ^ temp[10] ^ temp[12];
		bool b2 = temp[1] ^ temp[2] ^ temp[5] ^ temp[6] ^ temp[9] ^ temp[10];
		bool b3 = temp[3] ^ temp[4] ^ temp[5] ^ temp[6] ^ temp[11] ^ temp[12];
		bool b4 = temp[7] ^ temp[8] ^ temp[9] ^ temp[10] ^ temp[11] ^ temp[12];
		unsigned char status = b1 | (b2 << 1) | (b3 << 2) | (b4 << 3);
		if (status != 0 && status < 13) {
			temp[status - 1] = !temp[status - 1];
			flag = true;
		}
		if (temp[12] != (temp[2] ^ temp[4] ^ temp[5] ^ temp[6] ^ temp[8] ^ temp[9] ^ temp[10] ^ temp[11])) {
			delete[] content;
			return nullptr;
		}
		for (bool j: temp)
			newbits2.push_back(j);
	}
	for (int i = 0; i < newbits2.size(); i += 8) {
		content[i / 8] = util::get(&newbits2, i);
	}
	if (util::CRC(content, rest - 5) != crc) {
		delete[] content;
		return nullptr;
	} else if (flag)
		fprintf(stderr, "Hamming code correct!\n");
	for (int i = 0; i < newbits2.size(); i += 13) {
		temp[0] = newbits2[i];
		temp[1] = newbits2[i + 1];
		temp[2] = newbits2[i + 2];
		temp[3] = newbits2[i + 3];
		temp[4] = newbits2[i + 4];
		temp[5] = newbits2[i + 5];
		temp[6] = newbits2[i + 6];
		temp[7] = newbits2[i + 7];
		temp[8] = newbits2[i + 8];
		temp[9] = newbits2[i + 9];
		temp[10] = newbits2[i + 10];
		temp[11] = newbits2[i + 11];
		temp[12] = newbits2[i + 12];
		unsigned char c =
				(temp[2] << 7) | (temp[4] << 6) | (temp[5] << 5) | (temp[6] << 4) | (temp[8] << 3) | (temp[9] << 2) |
				(temp[10] << 1) | temp[11];
		content[i / 13] = c;
	}
	int rawLength = newbits.size() / 13;
	auto *ret = this->write(sequence, index, count, content, rawLength, wholeLength);
	delete[] content;
	return ret;
}

Block *FrameTable::write(int sequence, int index, int count, unsigned char *buffer, int len, int wholeLength) {
	this->check();
	auto time = util::getNowTime() + kFrameTime;
	if (this->table.find(sequence) == this->table.end())
		this->table.insert_or_assign(sequence, std::make_pair(time, std::map<int, std::vector<unsigned char>>()));
	std::vector<unsigned char> vector(buffer, buffer + len);
	auto &pair = this->table[sequence];
	auto &map = pair.second;
	map.insert_or_assign(index, vector);
	if (map.size() == count) {
		auto *block = new Block();
		for (auto &p: map)
			block->write(p.second.data(), p.second.size());
		block->flip(wholeLength);
		return block;
	}
	return nullptr;
}
