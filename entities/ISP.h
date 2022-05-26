#ifndef NETWORKDESIGN_ISP_H
#define NETWORKDESIGN_ISP_H


#include "NetworkEntity.h"
#include "network/MAC.h"
#include "AppLayer.h"
#include "Router.h"

class ISP : public NetworkEntity {

public:
	explicit ISP(Network* network);

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;

	bool isRouterMaster() override;

	bool isIPAvailable() override;

	IP * getIP() override;

	IP *allocateSegment(int node, IP segment, IP *pIp);
};

class RootAppLayer : public AppLayer {
public:

};


#endif //NETWORKDESIGN_ISP_H
