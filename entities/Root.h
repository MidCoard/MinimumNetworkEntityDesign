#ifndef NETWORKDESIGN_ROOT_H
#define NETWORKDESIGN_ROOT_H


#include "NetworkEntity.h"
#include "network/MAC.h"
#include "AppLayer.h"
#include "Router.h"

const MAC *kRootMAC = new MAC("ee:ee:ee:ee:ee:ee");
const IP *kRootIP = new IP("0.0.0.0");
const IP *kRootMask = new IP("0.0.0.0");

class Root : public NetworkEntity {

public:
	Root();

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;
};

class RootAppLayer : public AppLayer {
public:

};


#endif //NETWORKDESIGN_ROOT_H
