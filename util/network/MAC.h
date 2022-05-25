//
// Created by 周蜀杰 on 2022/5/23.
//

#ifndef NETWORKDESIGN_MAC_H
#define NETWORKDESIGN_MAC_H

#include "string"
#include <utility>
#include "random"

// new class will not be deleted in the whole process

class MAC {
public:
	explicit MAC(std::string mac);

private:
	std::string mac;
};

MAC generateMAC();


#endif //NETWORKDESIGN_MAC_H
