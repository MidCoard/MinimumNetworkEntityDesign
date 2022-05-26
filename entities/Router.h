#ifndef NETWORKDESIGN_ROUTER_H
#define NETWORKDESIGN_ROUTER_H

class Router;
class RouterConfiguration;
class RouterNetworkLayer;

#include "NetworkEntity.h"
#include "NetworkLayer.h"
#include "LinkLayer.h"
#include "PhysicalLayer.h"
#include "Network.h"
#include "network/IP.h"
#include "network/INetAddress.h"
#include "network/MAC.h"
#include "map"
#include "utility"

class RouterConfiguration {
public:
	RouterConfiguration(IP* segment, IP* mask, IP* gateway, MAC * mac, INetAddress * physicalAddress);

	IP* getSegment();
	IP* getMask();
	IP* getGateway();
	MAC* getMAC();
	INetAddress* getPhysicalAddress();
private:
	IP* segment;
	IP* mask;
	IP* gateway;
	MAC* mac;
	INetAddress* physicalAddress;
};

class Router : public NetworkEntity {
public:
	Router(Network* network, int node,IP* ip, MAC* mac, INetAddress* physicalAddress, std::map<int, RouterConfiguration> routerConfigurations);

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;

	bool isRouter() override;

	bool isRouterMaster() override;

	virtual void allocateIP() = 0;

	bool isIPAvailable() override;

	void setWAN(int port);

protected:
	std::map<int, RouterConfiguration> routerConfigurations;
	IP* ip;
	MAC* mac;
	INetAddress* physicalAddress;
	int wan = -1;
};

class RouterNetworkLayer : public NetworkLayer {
public:
	RouterNetworkLayer();

	explicit RouterNetworkLayer(int id);
};


#endif //NETWORKDESIGN_ROUTER_H
