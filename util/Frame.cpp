//
// Created by 周蜀杰 on 2022/4/18.
//

#include "Frame.h"

const unsigned char kFrameHeader = 0xf8;
const unsigned char kFrameFooter = 0xc6;
const unsigned char kFrameEscape = '\\';

// 1 + 20 + 1300 + 1 = 1322
const unsigned int kFrameSize = 1318; // byte

const int kFramePacketSize = 800; // byte



/**
 * one byte header 1byte
 * 4 byte sequence start
 * 4 byte sequenceNumber
 * 4 byte size
 * 4 byte length
 * packet content 800byte
 * // one byte + 1bit
 * // 9 bit -> Hamming code -> 4 bit
 * // 13 bit 1300byte
 * 4 byte CRC
 * one byte footer 1byte
 */


//

std::default_random_engine _generator;
std::uniform_int_distribution<int> _distribution(0, 2100000000);

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

int Frame::getSize() const {
	return this->size;
}

Frame::Frame(Block *block) {
	this->startSequenceNumber = _distribution(_generator);
	int len = block->getRemaining();
	auto* temp = new unsigned char[len];
	block->read(temp, len);
	for (int i = 0;i<len;i++) {
		if (temp[i] == kFrameHeader || temp[i] == kFrameFooter || temp[i] == kFrameEscape)
			this->data.push_back(kFrameEscape);
		this->data.push_back(temp[i]);
	}
	delete[] temp;
	this->size = this->data.size() % kFramePacketSize == 0 ? this->data.size() / kFramePacketSize : this->data.size() / kFramePacketSize + 1;
}

Block *Frame::createBlock(int pos) {
	if (pos >= this->size)
		return nullptr;
	auto* block = new Block();
	int actualSize = pos - 1 != this->size ? kFramePacketSize : this->data.size() - pos * kFramePacketSize;
	int len = actualSize % 8 == 0 ? actualSize : actualSize + 8 - actualSize % 8;
	std::bitset<kFramePacketSize * 13> bits;
	for (int i = 0;i<len;i++) {
		unsigned char c = i < actualSize ? this->data[pos * kFramePacketSize + i] : 0;
		// using 9 bits to get hamming code ( 8 bits data and one bit parity )
		bool a1 = c >> 7 & 1; // high bit
		bool a2 = c >> 6 & 1;
		bool a3 = c >> 5 & 1;
		bool a4 = c >> 4 & 1;
		bool a5 = c >> 3 & 1;
		bool a6 = c >> 2 & 1;
		bool a7 = c >> 1 & 1;
		bool a8 = c >> 0 & 1;
		bool a9 = a1 ^ a2 ^ a3 ^ a4 ^ a5 ^ a6 ^ a7 ^ a8; // low bit
		// generate 13 bits hamming code
		bits.set(i * 13 + 0, a9 ^ a8 ^ a6 ^ a5 ^ a3 ^ a1);
		bits.set(i * 13 + 1, a9 ^ a7 ^ a6 ^ a4 ^ a3);
		bits.set(i * 13 + 2, a9);
		bits.set(i * 13 + 3, a8 ^ a7 ^ a6 ^ a2 ^ a1);
		bits.set(i * 13 + 4, a8);
		bits.set(i * 13 + 5, a7);
		bits.set(i * 13 + 6, a6);
		bits.set(i * 13 + 7,  a5 ^ a4 ^ a3 ^ a2 ^ a1);
		bits.set(i * 13 + 8, a5);
		bits.set(i * 13 + 9, a4);
		bits.set(i * 13 + 10, a3);
		bits.set(i * 13 + 11, a2);
		bits.set(i * 13 + 12, a1);
	}
	// convert to unsigned char 8 bit
	int newLen = len / 8 * 13;
	auto* temp = new unsigned char[newLen];
	for (int i = 0;i<newLen;i++) {
		unsigned char c = 0;
		for (int j = 0;j<8;j++) {
			c <<= 1;
			c |= bits.test(i * 8 + j) ? 1 : 0;
		}
	}
	block->write(kFrameHeader);
	block->writeInt(this->startSequenceNumber);
	block->writeInt(this->startSequenceNumber + pos);
	block->writeInt(this->size);
	block->writeInt(actualSize);
	block->write(temp, newLen);
	block->writeInt(CRC(temp, newLen));
	block->write(kFrameFooter);
	delete[] temp;
	block->flip();
	return block;
}

Block *Frame::recreateBlock(int sequenceNumber) {
	return this->createBlock(sequenceNumber - startSequenceNumber);
}

int Frame::getStart() const {
	return this->startSequenceNumber;
}
