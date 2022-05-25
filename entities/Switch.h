//
// Created by 周蜀杰 on 2022/5/21.
//

#ifndef NETWORKDESIGN_SWITCH_H
#define NETWORKDESIGN_SWITCH_H

#include "NetworkEntity.h"
#include "network/MAC.h"
#include "network/INetAddress.h"
#include "LinkLayer.h"
#include "PhysicalLayer.h"
#include <utility>


class Switch : public NetworkEntity {

public:
	Switch(int node, MAC *mac);

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;
};


#endif //NETWORKDESIGN_SWITCH_H
