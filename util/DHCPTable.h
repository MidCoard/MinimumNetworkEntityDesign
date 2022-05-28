//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPTABLE_H
#define NETWORKDESIGN_DHCPTABLE_H

#include "vector"
#include "network/IP.h"
#include "map"
#include "random"

class DHCPTable {
public:
	DHCPTable(IP ip, IP mask);

	IP apply();

	std::pair<IP,IP> applySegment();

	bool apply(IP &ip);

	bool apply(IP &ip,IP &mask);

private:

	std::map<std::pair<IP,IP>, long long> segments;
	std::map<IP, long long > ips;

	std::map<std::pair<IP,IP>,long long> tempSegments;
	std::map<IP, long long> tempIps;

	IP ip;
	IP mask;

	void check();

};


#endif //NETWORKDESIGN_DHCPTABLE_H
