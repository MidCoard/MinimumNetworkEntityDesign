//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_APPLAYER_H
#define NETWORKDESIGN_APPLAYER_H

#include "Layer.h"

class AppLayer : public Layer{

public:
	std::string getName() override;
};


#endif //NETWORKDESIGN_APPLAYER_H
