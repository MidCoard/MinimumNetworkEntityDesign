//
// Created by 周蜀杰 on 2022/5/23.
//

#ifndef NETWORKDESIGN_IP_H
#define NETWORKDESIGN_IP_H

#include "string"
#include "Util.h"
#include <utility>

class IP {

public:
	explicit IP(std::string ip);
private:
	std::string ip;
	int bytes[4]{};
};


#endif //NETWORKDESIGN_IP_H
