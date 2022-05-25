//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_FRAME_H
#define NETWORKDESIGN_FRAME_H

#include "vector"
#include "network/INetAddress.h"

const char kFrameHeader = 0;
const char kFrameFooter = 0;
const char kFrameEscape = '\\';

// 1 + 20 + 1300 + 1 = 1322
const unsigned int kFrameSize = 1322; // byte


/**
 * one byte header 1byte
 *
 * sequence number 4byte
 * ip address (port) 6byte
 * packet size < 800 byte 2byte
 * 12 * 13 = 156 bit + 4(0x0000)bit = 160bit = 20byte
 *
 *
 * packet content 800byte
 * // one byte + 1bit
 * // 9 bit -> Hamming code -> 4 bit
 * // 13 bit 1300byte
 * one byte footer 1byte
 */


//

class Frame {
public:

	Frame(unsigned int sequenceNumber, const INetAddress &address, unsigned short size);

	~Frame();

	unsigned int getLength() const;

	unsigned int getSize() const;

	void putData(const char *data, unsigned int len);

private:
	char *data;
	unsigned int sequenceNumber;
	const INetAddress &address;
	unsigned short size;
	unsigned int length;
};

std::vector<Frame>
createFrames(unsigned int sequenceNumber, const char *data, unsigned int len, const INetAddress &address);


#endif //NETWORKDESIGN_FRAME_H
