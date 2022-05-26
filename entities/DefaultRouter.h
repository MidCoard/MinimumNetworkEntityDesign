//
// Created by 周蜀杰 on 2022/5/26.
//

#ifndef NETWORKDESIGN_DEFAULTROUTER_H
#define NETWORKDESIGN_DEFAULTROUTER_H


#include "Router.h"

class DefaultRouter : public Router {

public:
	DefaultRouter(Network *network, int node, IP* ip,MAC *mac,
	              INetAddress * physicalAddress, std::map<int, RouterConfiguration> routerConfigurations);

	void allocateIP() override;

	void dsfAllocateIP(int node, std::vector<bool> *visited, std::vector<IP *> *ips);
};


#endif //NETWORKDESIGN_DEFAULTROUTER_H
