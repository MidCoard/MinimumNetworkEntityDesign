#ifndef NETWORKDESIGN_NETWORKENTITY_H
#define NETWORKDESIGN_NETWORKENTITY_H

class Network;

#include "network/MAC.h"
#include "network/INetAddress.h"
#include "Layer.h"
#include "network/IPConfiguration.h"

class NetworkEntity {
public:
	NetworkEntity(Network* network,int node, Layer *layer);

	virtual ~NetworkEntity();

	virtual std::vector<std::string> createLayers(int node, std::vector<int> ids) = 0;

	virtual bool isRouter();

	virtual bool isRouterMaster();

	virtual bool isIPAvailable();

	virtual std::vector<IPConfiguration> getIPConfiguration();

protected:
	Layer *layer;
	int node;
	Network *network;
};


#endif //NETWORKDESIGN_NETWORKENTITY_H
