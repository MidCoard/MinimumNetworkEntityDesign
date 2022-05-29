//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPTABLE_H
#define NETWORKDESIGN_DHCPTABLE_H

#include "vector"
#include "network/IP.h"
#include "map"
#include "random"

extern const long long int kDHCPTime;

class DHCPTable {
public:
	DHCPTable(IP ip, IP mask);

	IP apply();

	std::pair<IP,IP> applySegment();

	bool apply(const IP &ip);

	bool apply(const IP &ip, const IP &mask);

	int dhcpID = 0;

	bool tryApply(const IP& ip,const IP& mask, int dhcpID);

	bool tryApply(const IP& ip, int dhcpID);

	bool renewal(const IP& ip, const IP& mask);

	bool renewal(const IP& ip);

private:

	std::map<std::pair<IP,IP>, long long> segments;
	std::map<IP, long long > ips;

	std::map<std::pair<IP,IP>,std::pair<long long,int>> tempSegments;
	std::map<IP, std::pair<long long,int>> tempIps;

	IP ip;
	IP mask;

	void check();

	bool applyDirect(const IP &ip, const IP &mask);

	bool applyDirect(const IP& ip);
};


#endif //NETWORKDESIGN_DHCPTABLE_H
