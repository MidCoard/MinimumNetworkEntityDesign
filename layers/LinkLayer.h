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
	LinkLayer();

	explicit LinkLayer(int id);

	void setMAC(int id, MAC *mac);

	std::string getRawName() override;

private:

	std::map<int, MAC *> macTable;

};


#endif //NETWORKDESIGN_LINKLAYER_H
