//
// Created by 周蜀杰 on 2022/5/21.
//

#ifndef NETWORKDESIGN_ROUTER_H
#define NETWORKDESIGN_ROUTER_H


#include "NetworkEntity.h"
#include "NetworkLayer.h"

class Router : public NetworkEntity {
public:
	Router(IP* segment, IP* mask, MAC* mac, INetAddress* address);
};

class RouterNetworkLayer: public NetworkLayer {
};



#endif //NETWORKDESIGN_ROUTER_H
