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

class LinkLayer : public Layer {
public:
	explicit LinkLayer(NetworkEntity * networkEntity);

	LinkLayer(int id, NetworkEntity * networkEntity);

	void setMAC(int id, const MAC& mac);

	std::string getRawName() override;

	void dealSend(Block block) override;

	void dealReceive(int id, Block block) override;

private:
	// not use pointer to avoid unnecessary delete operation
	std::map<MAC, int> macTable;

	std::map<int, MAC> idMacMap;

};


#endif //NETWORKDESIGN_LINKLAYER_H
