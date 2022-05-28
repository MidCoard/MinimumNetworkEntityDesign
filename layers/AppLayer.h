//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_APPLAYER_H
#define NETWORKDESIGN_APPLAYER_H

#include "Layer.h"

class AppLayer : public Layer {

public:
	explicit AppLayer(NetworkEntity * networkEntity);

	std::string getRawName() override;

	void handleReceive(int id, Block *block) override;

	void handleSend(Block *block) override;
};


#endif //NETWORKDESIGN_APPLAYER_H
