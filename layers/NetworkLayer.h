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
#include "ARPTable.h"


class NetworkLayer : public Layer {

public:
	explicit NetworkLayer(NetworkEntity * networkEntity);

	NetworkLayer(int id, NetworkEntity * networkEntity);

	void setIPConfiguration(int id, IP *segment, IP *mask, IP *gateway);

	IPConfiguration getIPConfiguration(int id);

	unsigned long long size();

	std::string getRawName() override;

	void dealReceive(int id, Block* block) override;

	void dealSend(Block* block) override;

protected:
	std::map<int, IPConfiguration> configurations;
	RouteTable routeTable;
	ARPTable arpTable;
	bool isIPValid = false;
	std::map<IP, MAC> ipMacMap;
};


#endif //NETWORKDESIGN_NETWORKLAYER_H
