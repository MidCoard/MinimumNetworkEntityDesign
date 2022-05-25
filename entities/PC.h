#ifndef NETWORKDESIGN_PC_H
#define NETWORKDESIGN_PC_H


#include "NetworkEntity.h"
#include "network/MAC.h"
#include "network/IP.h"
#include "network/INetAddress.h"
#include "AppLayer.h"
#include "NetworkLayer.h"
#include "LinkLayer.h"
#include "PhysicalLayer.h"
#include "Layer.h"

class PC : public NetworkEntity {

public:
	PC(int node, IP *ip, IP *gateway, MAC *mac, INetAddress *physicalAddress);

	~PC() override;

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;

private:
	IP *ip;
	IP *gateway;
	MAC *mac;
	INetAddress *physicalAddress;
};


#endif //NETWORKDESIGN_PC_H
