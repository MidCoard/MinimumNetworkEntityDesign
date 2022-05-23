#ifndef NETWORKDESIGN_ROOT_H
#define NETWORKDESIGN_ROOT_H


#include "NetworkEntity.h"
#include "network/MAC.h"

const MAC kRootMAC = MAC("ee:ee:ee:ee:ee:ee");
const IP kRootIP = IP("0.0.0.0");

class Root : public NetworkEntity {

public:
	Root();

};


#endif //NETWORKDESIGN_ROOT_H
