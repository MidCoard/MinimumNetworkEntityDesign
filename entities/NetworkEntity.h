#ifndef NETWORKDESIGN_NETWORKENTITY_H
#define NETWORKDESIGN_NETWORKENTITY_H


#include "network/MAC.h"
#include "network/INetAddress.h"
#include "Layer.h"

class NetworkEntity {
public:
	NetworkEntity(MAC* mac, INetAddress* physicalAddress);
	~NetworkEntity();
protected:
	const MAC* mac;
	const INetAddress* physicalAddress;
	Layer * layer = nullptr;
};


#endif //NETWORKDESIGN_NETWORKENTITY_H
