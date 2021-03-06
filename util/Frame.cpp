//
// Created by 周蜀杰 on 2022/4/18.
//

#include "Frame.h"
#include "chrono"

const unsigned char kFrameHeader = 0xf8;
const unsigned char kFrameFooter = 0xc6;
const unsigned char kFrameEscape = '\\';

const int kFramePacketSize = 13000; // byte

/**
 * one byte header 1byte
 * 4 byte sequence start
 * 4 byte sequenceNumber
 * 4 byte size
 * 4 byte length
 * 4 byte whole length
 * packet content 800byte
 * // one byte + 1bit
 * // 9 bit -> Hamming code -> 4 bit
 * // 13 bit 1300byte
 * 4 byte CRC
 * one byte footer 1byte
 *
 * and kFrameEscape should be considered
 */

std::default_random_engine _generator(std::chrono::system_clock::now().time_since_epoch().count());
std::uniform_int_distribution<int> _distribution(0, 2100000000);

int Frame::getSize() const {
	return this->size;
}

Frame::Frame(Block *block) {
	this->startSequenceNumber = _distribution(_generator);
	std::vector<bool> bits;
	std::vector<bool> newbits;
	this->length = block->getRemaining();
	int newLength = length % 8 == 0 ? length : length + 8 - length % 8;
	newLength = newLength / 8 * 13;
	while (block->getRemaining())
		util::put(&bits, block->readUnsignChar());
	bool temp[13];
	for (int i = 0; i < bits.size(); i += 8) {
		temp[2] = bits[i]; //high bit
		temp[4] = bits[i + 1];
		temp[5] = bits[i + 2];
		temp[6] = bits[i + 3];
		temp[8] = bits[i + 4];
		temp[9] = bits[i + 5];
		temp[10] = bits[i + 6];
		temp[11] = bits[i + 7];// low bit
		temp[12] = temp[2] ^ temp[4] ^ temp[5] ^ temp[6] ^ temp[8] ^ temp[9] ^ temp[10] ^ temp[11];
		temp[0] = temp[2] ^ temp[4] ^ temp[6] ^ temp[8] ^ temp[10] ^ temp[12];
		temp[1] = temp[2] ^ temp[5] ^ temp[6] ^ temp[9] ^ temp[10];
		temp[3] = temp[4] ^ temp[5] ^ temp[6] ^ temp[11] ^ temp[12];
		temp[7] = temp[8] ^ temp[9] ^ temp[10] ^ temp[11] ^ temp[12];
		for (bool &j: temp)
			newbits.push_back(j);
	}
	while (newbits.size() < newLength * 8)
		newbits.push_back(false);
	for (int i = 0; i < newLength; i++) {
		unsigned char c = 0;
		for (int j = 0; j < 8; j++)
			c |= newbits[i * 8 + j] << (7 - j);
		this->data.push_back(c);
	}
	this->size = this->data.size() % kFramePacketSize == 0 ? this->data.size() / kFramePacketSize :
	             this->data.size() / kFramePacketSize + 1;
}

Block *Frame::createBlock(int pos) {
	if (pos >= this->size)
		return nullptr;
	auto *block = new Block();
	int actualSize = pos != this->size - 1 ? kFramePacketSize : this->data.size() - pos * kFramePacketSize;
	block->writeInt(this->startSequenceNumber);
	block->writeInt(pos);
	block->writeInt(this->size);
	block->writeInt(this->length);
	block->write(this->data.data() + pos * kFramePacketSize, actualSize);
	block->writeInt(util::CRC(this->data.data() + pos * kFramePacketSize, actualSize));
	block->flip();
	auto *ret = new Block();
	ret->write(kFrameHeader);
	while (block->getRemaining()) {
		unsigned char c = block->readUnsignChar();
		if (c == kFrameEscape || c == kFrameHeader || c == kFrameFooter)
			ret->write(kFrameEscape);
		ret->write(c);
	}
	ret->write(kFrameFooter);
	ret->flip();
	return ret;
}

Block *Frame::recreateBlock(int sequenceNumber) {
	return this->createBlock(sequenceNumber - startSequenceNumber);
}

int Frame::getStart() const {
	return this->startSequenceNumber;
}
