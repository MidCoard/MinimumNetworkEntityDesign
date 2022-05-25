//
// Created by 周蜀杰 on 2022/5/21.
//

#ifndef NETWORKDESIGN_SWITCH_H
#define NETWORKDESIGN_SWITCH_H

#include "NetworkEntity.h"
#include "network/MAC.h"
#include "network/INetAddress.h"


class Switch : public NetworkEntity{

public:
	Switch(MAC* mac, INetAddress* physicalAddress);
};


#endif //NETWORKDESIGN_SWITCH_H
