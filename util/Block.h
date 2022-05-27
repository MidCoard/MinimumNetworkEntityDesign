//
// Created by 周蜀杰 on 2022/5/27.
//

#ifndef NETWORKDESIGN_BLOCK_H
#define NETWORKDESIGN_BLOCK_H

#include "vector"

class Block {
public:
	Block();

	~Block();

	void flip();

	void write(unsigned char *data, int len);

	int getRemaining() const;

	void read(unsigned char *data, int len);

	std::vector<unsigned char> getData();

private:
	std::vector<unsigned char> * temp;
	unsigned char *data;
	int remaining;
	int pos;
};


#endif //NETWORKDESIGN_BLOCK_H
