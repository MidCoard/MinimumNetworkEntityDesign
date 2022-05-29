#ifndef NETWORKDESIGN_ISP_H
#define NETWORKDESIGN_ISP_H

class Network;

#include "network/MAC.h"
#include "AppLayer.h"
#include "LinkLayer.h"
#include "Router.h"
#include "PhysicalLayer.h"

class ISP : public NetworkEntity {

public:
	explicit ISP(Network *network);

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;

	void start() override;

	RouterNetworkLayer *networkLayer;

	std::string getName() override;

	void generateIP() override;

	bool generatedIP = false;

	void dfsAllocateIP(int node, std::vector<bool> *visited, std::vector<IPConfiguration> *configurations);
};

class RootAppLayer : public AppLayer {
public:
	explicit RootAppLayer(NetworkEntity *networkEntity);
};


class EmptyLinkLayer : public LinkLayer {
public:
	EmptyLinkLayer(int id, NetworkEntity *networkEntity);

	void start() override;

	void stop() override;

	void handleReceive(int id, Block *block) override;

	void handleSend(Block *block) override;
};

class EmptyPhysicalLayer : public PhysicalLayer {
	void start() override;

	void stop() override;

	void handleReceive(int id, Block *block) override;

	void handleSend(Block *block) override;

public:
	EmptyPhysicalLayer(int id, NetworkEntity *networkEntity);
};


#endif //NETWORKDESIGN_ISP_H
