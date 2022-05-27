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

class RouterConfiguration {
public:
	RouterConfiguration(IP *segment, IP *mask, IP *gateway, MAC *mac, INetAddress *physicalAddress,
	                    INetAddress *pAddress);
	~RouterConfiguration();
	IP* getSegment();
	IP* getMask();
	IP* getGateway();
	MAC* getMAC();
	INetAddress* getLinkAddress();
	INetAddress* getPhysicalAddress();
private:
	IP* segment;
	IP* mask;
	IP* gateway;
	MAC* mac;
	INetAddress* linkAddress;
	INetAddress* physicalAddress;
};

class Router : public NetworkEntity {
public:
	Router(Network* network, int node,std::map<int, RouterConfiguration> routerConfigurations);

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;

	bool isRouter() override;

	virtual void generateIP() = 0;

	bool isIPAvailable() override;

	std::vector<IPConfiguration> getIPConfiguration() override = 0;
protected:
	std::map<int, RouterConfiguration> routerConfigurations;
	bool generatedIP = false;
};

class RouterNetworkLayer : public NetworkLayer {
public:
	RouterNetworkLayer();

	explicit RouterNetworkLayer(int id);

	IPConfiguration getIPConfiguration(int id);

	unsigned long size();
};


#endif //NETWORKDESIGN_ROUTER_H
