//
// Created by 周蜀杰 on 2022/5/23.
//

#ifndef NETWORKDESIGN_MAC_H
#define NETWORKDESIGN_MAC_H

#include "string"


// new class will not be deleted in the whole process

class MAC {
public:
	MAC(std::string mac);

private:
	const std::string mac;
};


#endif //NETWORKDESIGN_MAC_H
