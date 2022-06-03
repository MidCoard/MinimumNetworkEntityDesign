#ifndef NETWORKDESIGN_ROUTER_H
#define NETWORKDESIGN_ROUTER_H

class Network;

#include "NetworkLayer.h"
#include "LinkLayer.h"
#include "PhysicalLayer.h"
#include "network/IP.h"
#include "network/INetAddress.h"
#include "network/MAC.h"
#include "network/IPConfiguration.h"
#include "map"
#include "utility"
#include "NetworkEntity.h"
#include "DHCPTable.h"

class RouterConfiguration {
public:
	RouterConfiguration(IP *segment, IP *mask, IP *gateway, MAC *mac, INetAddress *physicalAddress,
	                    INetAddress *pAddress);

	~RouterConfiguration();

	IP *getSegment();

	IP *getMask();

	IP *getGateway();

	MAC *getMAC();

	INetAddress *getLinkAddress();

	INetAddress *getPhysicalAddress();

private:
	IP *segment;
	IP *mask;
	IP *gateway;
	MAC *mac;
	INetAddress *linkAddress;
	INetAddress *physicalAddress;
};

class Router : public NetworkEntity {
public:
	Router(Network *network, int node, std::map<int, RouterConfiguration *> routerConfigurations);

	~Router() override;

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;

	bool isIPAvailable() override;

	void start() override;

protected:
	std::map<int, RouterConfiguration *> routerConfigurations;
	bool generatedIP = false;

};

class RouterNetworkLayer : public NetworkLayer {
public:
	explicit RouterNetworkLayer(NetworkEntity *networkEntity);

	~RouterNetworkLayer() override;

	RouterNetworkLayer(int id, NetworkEntity *networkEntity);

	void handleReceive(int id, Block *block) override;

	void sendDHCP() override;

	IP getIP(int id) override;

	std::vector<DHCPTable *> tables;

	void sendDHCPRelease() override;
};


#endif //NETWORKDESIGN_ROUTER_H
