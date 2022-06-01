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
	SwitchConfiguration(INetAddress *linkAddress, INetAddress *physicalAddress);

	~SwitchConfiguration();

	INetAddress *getPhysicalAddress();

	INetAddress *getLinkAddress();

private:
	INetAddress *linkAddress;
	INetAddress *physicalAddress;
};


class Switch : public NetworkEntity {

public:
	Switch(Network *network, int node, std::map<int, SwitchConfiguration *> switchConfigurations);

	~Switch() override;

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;

	std::string getName() override;

private:
	std::map<int, SwitchConfiguration *> switchConfigurations;
};

class SwitchLinkLayer : public LinkLayer {
public:
	explicit SwitchLinkLayer(NetworkEntity *networkEntity);

	void handleReceive(int id, Block *block) override;

	void handleSend(Block *block) override;
private:

	MACTable macTable;

};

#endif //NETWORKDESIGN_SWITCH_H
