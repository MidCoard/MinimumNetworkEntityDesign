#ifndef NETWORKDESIGN_NETWORKENTITY_H
#define NETWORKDESIGN_NETWORKENTITY_H


#include "network/MAC.h"
#include "network/INetAddress.h"
#include "Layer.h"

class NetworkEntity {
public:
	NetworkEntity(int node, Layer *layer);

	virtual ~NetworkEntity();

	virtual std::vector<std::string> createLayers(int node, std::vector<int> ids) = 0;

protected:
	Layer *layer;
	int node;
};


#endif //NETWORKDESIGN_NETWORKENTITY_H
