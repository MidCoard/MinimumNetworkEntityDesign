//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_NETWORKLAYER_H
#define NETWORKDESIGN_NETWORKLAYER_H

#include "network/IP.h"
#include "Layer.h"


class NetworkLayer : public Layer{

public:
	NetworkLayer(IP* ip, IP* gateway);
	NetworkLayer(int id, IP* ip, IP* gateway);
	std::string getName() override;
private:
	IP* ip;
	IP* gateway;
};


#endif //NETWORKDESIGN_NETWORKLAYER_H
