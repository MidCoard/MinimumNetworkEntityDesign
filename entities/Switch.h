#ifndef NETWORKDESIGN_SWITCH_H
#define NETWORKDESIGN_SWITCH_H

class Network;

#include "NetworkEntity.h"
#include "network/MAC.h"
#include "network/INetAddress.h"
#include "LinkLayer.h"
#include "PhysicalLayer.h"
#include <utility>
#include "map"


class SwitchConfiguration {
public:
	// pointer to present null
	SwitchConfiguration(MAC *mac, INetAddress *linkAddress, INetAddress *physicalAddress);
	~SwitchConfiguration();
	MAC * getMAC();
	INetAddress * getPhysicalAddress();
	INetAddress * getLinkAddress();
private:
	MAC * mac;
	INetAddress * linkAddress;
	INetAddress * physicalAddress;
};


class Switch : public NetworkEntity {

public:
	Switch(Network * network, int node, std::map<int, SwitchConfiguration> switchConfigurations);

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;
private:
	std::map<int, SwitchConfiguration> switchConfigurations;
};

#endif //NETWORKDESIGN_SWITCH_H
