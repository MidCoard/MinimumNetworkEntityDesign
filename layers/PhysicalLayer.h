//
// Created by 周蜀杰 on 2022/5/9.
//

#ifndef NETWORKDESIGN_PHYSICALLAYER_H
#define NETWORKDESIGN_PHYSICALLAYER_H

#include "Layer.h"
#include "network/INetAddress.h"
#include "queue"


class PhysicalLayer : public Layer {
public:
	PhysicalLayer(INetAddress* linkAddress, INetAddress * physicalAddress);

	PhysicalLayer(int id, INetAddress* linkAddress, INetAddress * physicalAddress);

	std::string getRawName() override;

	void start();

private:
	INetAddress *physicalAddress;
	INetAddress *linkAddress;

	Socket *socket;
};


#endif //NETWORKDESIGN_PHYSICALLAYER_H
