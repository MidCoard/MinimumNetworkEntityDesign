#ifndef NETWORKDESIGN_ISP_H
#define NETWORKDESIGN_ISP_H

class Network;

#include "network/MAC.h"
#include "AppLayer.h"
#include "Router.h"

class ISP : public NetworkEntity {

public:
	explicit ISP(Network* network);

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;

	void start() override;
};

class RootAppLayer : public AppLayer {
public:

};


#endif //NETWORKDESIGN_ISP_H
