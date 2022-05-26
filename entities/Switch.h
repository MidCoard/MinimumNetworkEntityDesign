#ifndef NETWORKDESIGN_SWITCH_H
#define NETWORKDESIGN_SWITCH_H

#include "NetworkEntity.h"
#include "network/MAC.h"
#include "network/INetAddress.h"
#include "LinkLayer.h"
#include "PhysicalLayer.h"
#include <utility>
#include "map"


class SwitchConfiguration {
public:
	SwitchConfiguration(MAC * mac, INetAddress * address);
	MAC * getMAC();
	INetAddress * getPhysicalAddress();
private:
	MAC * mac;
	INetAddress * address;
};


class Switch : public NetworkEntity {

public:
	Switch(Network * network, int node, std::map<int, SwitchConfiguration> switchConfigurations);

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;
private:
	std::map<int, SwitchConfiguration> switchConfigurations;
};

#endif //NETWORKDESIGN_SWITCH_H
