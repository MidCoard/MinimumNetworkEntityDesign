//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_LINKLAYER_H
#define NETWORKDESIGN_LINKLAYER_H

#include "network/INetAddress.h"
#include "Frame.h"
#include "Layer.h"
#include "network/MAC.h"
#include <vector>
#include <utility>
#include "map"
#include "MACTable.h"

class LinkLayer : public Layer {
public:
	explicit LinkLayer(NetworkEntity * networkEntity);

	LinkLayer(int id, NetworkEntity * networkEntity);

	void setMAC(int id, const MAC& mac);

	std::string getRawName() override;

	void dealSend(Block* block) override;

	void dealReceive(int id, Block* block) override;

private:

	// facing a problem when one interface have one mac, so use a map to store the mac
	// in fact it is unnecessary for our now design
	std::map<int, MAC> idMacMap;
};


#endif //NETWORKDESIGN_LINKLAYER_H
