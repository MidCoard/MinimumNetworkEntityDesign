//
// Created by 周蜀杰 on 2022/6/8.
//

#ifndef NETWORKDESIGN_HOMEROUTER_H
#define NETWORKDESIGN_HOMEROUTER_H


#include "Router.h"

class HomeRouter : public Router {
public:
	HomeRouter(Network *network, int node, std::map<int, RouterConfiguration *> map);

	void generateIP() override;

	std::string getName() override;

	std::vector<IPConfiguration> getIPConfiguration() override;

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;
};

class HomeRouterNetworkLayer : public RouterNetworkLayer {

	void handleReceive(int id, Block *block) override;

	void handleSend(Block *block) override;

public:
	explicit HomeRouterNetworkLayer(NetworkEntity * entity);

private:
	// for privateIP to publicIP
	std::map<IP,IP> natMap;
	// for publicIP to privateIP
	std::map<IP,IP> natMapReverse;
	DHCPTable* natTable = nullptr;

};


#endif //NETWORKDESIGN_HOMEROUTER_H
