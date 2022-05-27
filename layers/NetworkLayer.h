//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_NETWORKLAYER_H
#define NETWORKDESIGN_NETWORKLAYER_H

#include "network/IP.h"
#include "Layer.h"
#include "map"
#include "network/IPConfiguration.h"
#include "RouteTable.h"
#include "network/MAC.h"


class NetworkLayer : public Layer {

public:
	NetworkLayer();

	explicit NetworkLayer(int id);

	void setIPConfiguration(int id, IP *segment, IP *mask, IP *gateway);

	std::string getRawName() override;

	void dealReceive(int id, Block block) override;

	void dealSend(Block block) override;

protected:
	std::map<int, IPConfiguration> configurations;
	RouteTable routeTable;
	bool isIPValid = false;
	std::map<IP, MAC> ipMacMap;
};


#endif //NETWORKDESIGN_NETWORKLAYER_H
