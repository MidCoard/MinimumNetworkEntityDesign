#ifndef NETWORKDESIGN_NETWORKENTITY_H
#define NETWORKDESIGN_NETWORKENTITY_H


#include "network/MAC.h"
#include "network/INetAddress.h"
#include "Layer.h"

class NetworkEntity {
public:
	explicit NetworkEntity(Layer * layer);
	~NetworkEntity();

	virtual void createLayers(std::vector<int> ids) = 0;
protected:
	Layer * layer;
};


#endif //NETWORKDESIGN_NETWORKENTITY_H
