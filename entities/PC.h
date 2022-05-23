#ifndef NETWORKDESIGN_PC_H
#define NETWORKDESIGN_PC_H


#include "NetworkEntity.h"

class PC : public NetworkEntity {


public:
	PC(IP *ip, IP *gateway, struct MAC *mac, INetAddress *physicalAddress);
};


#endif //NETWORKDESIGN_PC_H
