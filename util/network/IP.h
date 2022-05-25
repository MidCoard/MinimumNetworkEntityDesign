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

private:
	std::string ip;
	unsigned char bytes[4]{};
};

extern IP localhost;


#endif //NETWORKDESIGN_IP_H
