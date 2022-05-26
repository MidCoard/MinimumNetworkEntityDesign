//
// Created by 周蜀杰 on 2022/5/23.
//

#ifndef NETWORKDESIGN_IP_H
#define NETWORKDESIGN_IP_H

#include "string"
#include "Util.h"
#include <utility>

// new class will not be deleted in the whole process

class IP {
public:
	explicit IP(std::string ip);
	explicit IP(unsigned char bytes[]);
	explicit IP(unsigned int ip);
	IP(unsigned char a, unsigned char b, unsigned char c, unsigned char d);

	IP operator &(IP &ip);

	bool operator ==(IP &ip);

	bool operator !=(IP &ip);

	std::string str();

	IP getMask();

private:
	std::string ip;
	unsigned char bytes[4]{};
};

extern IP localhost;


#endif //NETWORKDESIGN_IP_H
