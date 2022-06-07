//
// Created by 周蜀杰 on 2022/6/8.
//

#ifndef NETWORKDESIGN_HOMEROUTER_H
#define NETWORKDESIGN_HOMEROUTER_H


#include "Router.h"

class HomeRouter : public Router {
public:
	HomeRouter(Network *network, int node);

	std::string getName() override;
};

class HomeRouterNetworkLayer : public NetworkLayer {

	void handleReceive(int id, Block *block) override;

public:
	explicit HomeRouterNetworkLayer(NetworkEntity * entity);
};


#endif //NETWORKDESIGN_HOMEROUTER_H
