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
	PhysicalLayer(NetworkEntity *networkEntity, INetAddress linkAddress, INetAddress physicalAddress);

	PhysicalLayer(int id, NetworkEntity *networkEntity, INetAddress linkAddress, INetAddress physicalAddress);

	std::string getRawName() override;

	void start() override;

	void stop() override;

	void handleReceive(int id, Block *block) override;

	void handleSend(Block *block) override;

private:
	INetAddress physicalAddress;
	INetAddress linkAddress;


	// why use pointer?
	// because we need to know the socket is used or not(nullptr)
	Socket *socket = nullptr;
};


#endif //NETWORKDESIGN_PHYSICALLAYER_H
