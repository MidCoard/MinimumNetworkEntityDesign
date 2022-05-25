//
// Created by 周蜀杰 on 2022/5/9.
//

#ifndef NETWORKDESIGN_PHYSICALLAYER_H
#define NETWORKDESIGN_PHYSICALLAYER_H

#include "Layer.h"
#include "network/INetAddress.h"


class PhysicalLayer : public Layer {
public:
	explicit PhysicalLayer(INetAddress* iNetAddress);
	PhysicalLayer(int id, INetAddress* iNetAddress);
	std::string getName() override;
private:
	const INetAddress* iNetAddress;
};


#endif //NETWORKDESIGN_PHYSICALLAYER_H
