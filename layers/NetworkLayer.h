//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_NETWORKLAYER_H
#define NETWORKDESIGN_NETWORKLAYER_H

#include "network/IP.h"
#include "Layer.h"
#include "map"


class NetworkLayer : public Layer {

public:
	NetworkLayer();

	explicit NetworkLayer(int id);

	void setIP(int id, IP *ip, IP *mask, IP *gateway);

	std::string getRawName() override;

private:
	std::map<int, IP*> ips;
	std::map<int, IP*> masks;
	std::map<int, IP*> gateways;
};


#endif //NETWORKDESIGN_NETWORKLAYER_H
