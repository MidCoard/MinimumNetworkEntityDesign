//
// Created by 周蜀杰 on 2022/5/27.
//

#ifndef NETWORKDESIGN_BLOCK_H
#define NETWORKDESIGN_BLOCK_H

#include "vector"

class Block {
public:
	Block();

	void flip();

	void write(unsigned char *data, int len);

	int getRemaining() const;

	int read(unsigned char *data, int len);

	std::vector<unsigned char> getData();

private:
	// avoid use pointer
	std::vector<unsigned char> temp;
	int remaining;
	int pos;
};


#endif //NETWORKDESIGN_BLOCK_H
