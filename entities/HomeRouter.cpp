//
// Created by 周蜀杰 on 2022/6/8.
//

#include "HomeRouter.h"

#include <utility>

void HomeRouterNetworkLayer::handleReceive(int id, Block *block) {

}

HomeRouterNetworkLayer::HomeRouterNetworkLayer(NetworkEntity *entity) : NetworkLayer(entity) {}

std::string HomeRouter::getName() {
	return "HomeRouter";
}

HomeRouter::HomeRouter(Network *network, int node) : Router(network,node,new HomeRouterNetworkLayer(this)) {}
