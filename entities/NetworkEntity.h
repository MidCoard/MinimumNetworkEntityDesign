#ifndef NETWORKDESIGN_NETWORKENTITY_H
#define NETWORKDESIGN_NETWORKENTITY_H

class Network;

#include "network/MAC.h"
#include "network/INetAddress.h"
#include "Layer.h"
#include "network/IPConfiguration.h"
#include "Executor.h"

extern const Executor kExecutor;

class NetworkEntity {
public:
	NetworkEntity(Network* network,int node, Layer* layer);

	virtual ~NetworkEntity();

	virtual std::vector<std::string> createLayers(int node, std::vector<int> ids) = 0;

	virtual bool isRouter();

	virtual bool isIPAvailable();

	virtual std::vector<IPConfiguration> getIPConfiguration();

	virtual void start();

	virtual void stop();

	virtual std::string getName() = 0;

protected:
	Layer * layer;
	int node;
	Network * network;
	bool isStarted = false;
	bool isStopped = false;
};


#endif //NETWORKDESIGN_NETWORKENTITY_H
