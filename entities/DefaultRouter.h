//
// Created by 周蜀杰 on 2022/5/26.
//

#ifndef NETWORKDESIGN_DEFAULTROUTER_H
#define NETWORKDESIGN_DEFAULTROUTER_H

class DefaultRouter;

#include "Router.h"
#include "network/IPConfiguration.h"

class DefaultRouter : public Router {

public:
	DefaultRouter(Network *network, int node,  std::map<int, RouterConfiguration *> routerConfigurations);

	void generateIP() override;

	void dfsAllocateIP(int node, std::vector<bool> *visited, std::vector<IPConfiguration> *configurations);

	std::vector<IPConfiguration> getIPConfiguration() override;

	std::string getName() override;
};


#endif //NETWORKDESIGN_DEFAULTROUTER_H
