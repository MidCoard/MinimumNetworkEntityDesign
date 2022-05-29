//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_NETWORKLAYER_H
#define NETWORKDESIGN_NETWORKLAYER_H

#include "network/IP.h"
#include "network/IPConfiguration.h"
#include "RouteTable.h"
#include "ARPTable.h"
#include "LinkLayer.h"
#include "ICMPTable.h"
#include "UDPTable.h"


class NetworkLayer : public Layer {

public:
	explicit NetworkLayer(NetworkEntity *networkEntity);

	NetworkLayer(int id, NetworkEntity *networkEntity);

	void setIPConfiguration(int id, IP *segment, IP *mask, IP *gateway);

	IPConfiguration getIPConfiguration(int id);

	unsigned long long size();

	std::string getRawName() override;

	void handleReceive(int id, Block *block) override;

	void handleSend(Block *block) override;

	IP getIP();

	bool isIPValid = false;

	void handleARP(const IP &ip, const MAC &mac);

	virtual void sendDHCP();

	virtual void sendDHCPRenewal();

	void sendDHCP0(bool useSegment);

	void sendDHCPRenewal0(bool useSegment);

	void sendICMP(IP ip);

protected:
	std::map<int, IPConfiguration> configurations;
	RouteTable routeTable;
	ARPTable arpTable;
	std::map<IP, MAC> ipMacMap;
	ICMPTable icmpTable;

	int dhcpID = -1;
	long long int duration = 0;
	long long int startDHCP = 0;

	void checkDHCP();
};


#endif //NETWORKDESIGN_NETWORKLAYER_H
