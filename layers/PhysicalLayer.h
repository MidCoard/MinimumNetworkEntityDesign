//
// Created by 周蜀杰 on 2022/5/9.
//

#ifndef NETWORKDESIGN_PHYSICALLAYER_H
#define NETWORKDESIGN_PHYSICALLAYER_H

#include "Layer.h"
#include "network/INetAddress.h"
#include "queue"
#include "Socket.h"


class PhysicalLayer : public Layer {
public:
	PhysicalLayer(INetAddress* linkAddress, INetAddress * physicalAddress);

	PhysicalLayer(int id, INetAddress* linkAddress, INetAddress * physicalAddress);

	std::string getRawName() override;

	void start() override;

	void stop() override;

private:
	INetAddress *physicalAddress;
	INetAddress *linkAddress;

	Socket *socket;

	void deal(Block *pBlock);
};


#endif //NETWORKDESIGN_PHYSICALLAYER_H
