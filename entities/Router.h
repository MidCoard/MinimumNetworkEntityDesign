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
	Router(Network* network, int node,std::map<int, RouterConfiguration*> routerConfigurations);

	~Router() override;

	std::vector<std::string> createLayers(int node, std::vector<int> ids) override;

	bool isRouter() override;

	virtual void generateIP() = 0;

	bool isIPAvailable() override;

	void start() override;

	// first set in generateIP
	// second set in DHCP server ( this time should delete the first set )
	IP* segment = nullptr;
	IP* mask = nullptr;
	IP* gateway = nullptr;
protected:
	std::map<int, RouterConfiguration*> routerConfigurations;
	bool generatedIP = false;

	// mark all
};

class RouterNetworkLayer : public NetworkLayer {
public:
	explicit RouterNetworkLayer(NetworkEntity * networkEntity);

	~RouterNetworkLayer() override;

	RouterNetworkLayer(int id, NetworkEntity * networkEntity);

	void handleReceive(int id, Block *block) override;

	void sendDHCP() override;
	DHCPTable * table = nullptr;
};


#endif //NETWORKDESIGN_ROUTER_H
