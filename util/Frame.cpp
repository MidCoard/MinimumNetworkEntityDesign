//
// Created by 周蜀杰 on 2022/4/18.
//

#include "Frame.h"


Frame::Frame(unsigned int sequenceNumber, const INetAddress &address, unsigned short size) : sequenceNumber(
		sequenceNumber), address(address), size(size) {
	this->data = new char[kFrameSize];
	this->length = 22 + (size * 13 % 8 == 0 ? size * 13 / 8 : size * 13 / 8 + 1);
//	this->data[0] =
}

Frame::~Frame() {
	delete[] this->data;
}

unsigned int Frame::getLength() const {
	return this->length;
}

unsigned int Frame::getSize() const {
	return this->size;
}

void Frame::putData(const char *data, unsigned int len) {
	memcpy(this->data, data, len);
}

std::vector<Frame>
createFream(unsigned int sequenceNumber, const char *data, unsigned int len, const INetAddress &address) {
	std::vector<Frame> frames = std::vector<Frame>();
	int frameCount = len % kFrameSize == 0 ? len / kFrameSize : len / kFrameSize + 1;
	for (int i = 0; i < frameCount; i++) {
		Frame frame = Frame(sequenceNumber, address,
		                    len - i * kFrameSize > kFrameSize ? kFrameSize : len - i * kFrameSize);
		frame.putData(data + i * kFrameSize, frame.getSize());
		frames.push_back(frame);
	}
	return frames;
}
