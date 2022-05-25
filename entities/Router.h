//
// Created by 周蜀杰 on 2022/5/21.
//

#ifndef NETWORKDESIGN_ROUTER_H
#define NETWORKDESIGN_ROUTER_H


#include "NetworkEntity.h"
#include "NetworkLayer.h"
#include "LinkLayer.h"
#include "PhysicalLayer.h"

class Router : public NetworkEntity {
public:
	Router(IP* segment, IP* mask);
	void createLayers(int node, std::vector<int> ids) override;
private:
	IP *segment;
	IP *mask;
};

class RouterNetworkLayer: public NetworkLayer {

public:
	RouterNetworkLayer(IP * segment, IP * mask);
};



#endif //NETWORKDESIGN_ROUTER_H
