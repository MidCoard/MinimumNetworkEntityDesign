//
// Created by 周蜀杰 on 2022/5/27.
//

#ifndef NETWORKDESIGN_BLOCK_H
#define NETWORKDESIGN_BLOCK_H


class Block {
public:
	Block(unsigned char * data, int len);
private:
	unsigned char *data;
	int len;
};


#endif //NETWORKDESIGN_BLOCK_H
