#ifndef NETWORKDESIGN_PC_H
#define NETWORKDESIGN_PC_H

class Network;

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
	PC(Network *network, int node, IP *ip, IP *mask, IP *gateway, MAC *mac, INetAddress *physicalAddress,
	   INetAddress *pAddress);

	~PC() override;

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;

	bool isIPAvailable() override;

	std::vector<IPConfiguration> getIPConfiguration() override;

	void start() override;

private:
	IP *ip;
	IP *mask;
	IP *gateway;
	MAC *mac;
	INetAddress *linkAddress;
	INetAddress *physicalAddress;
};


#endif //NETWORKDESIGN_PC_H
