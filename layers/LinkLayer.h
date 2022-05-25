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
#include <utility>

class LinkLayer : public Layer{
public:
	explicit LinkLayer(MAC* mac);
	LinkLayer(int id,MAC* mac);
	std::string getName() override;

private:
	const MAC* mac;

};


#endif //NETWORKDESIGN_LINKLAYER_H
