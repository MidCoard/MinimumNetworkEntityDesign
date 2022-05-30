//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPTABLE_H
#define NETWORKDESIGN_DHCPTABLE_H

#include "vector"
#include "network/IP.h"
#include "map"
#include "random"
#include "network/MAC.h"
#include "bitset"

extern const long long int kDHCPTime;


class DHCPTable {

	class TableItem {
	public:
		TableItem(long long int start, long long int last);

		bool operator<(const TableItem &item) const;

		long long int start;

		[[nodiscard]] long long int right() const;

		[[nodiscard]] long long int left() const;

	private:
		long long int last;
	};

public:
	DHCPTable(IP ip, IP mask);

	std::pair<IP, long long int> apply0();

	std::pair<std::pair<IP, IP>, long long int> applySegment();
	std::pair<std::pair<IP, IP>, long long int> applySegment0();

	unsigned long long int apply(const IP& ip);

	std::pair<IP, IP>directApplySegment(const MAC& mac);

	bool directApplySegment(const IP&ip, const IP&mask, const MAC& mac);

	long long int apply(const IP &ip, const IP &mask);

	IP directApply(const MAC& mac);

	bool directApply(const IP&ip, const MAC& mac);

	bool applyIt(IP* ip, IP* mask, const MAC& mac, int id);

	bool applyIt(const IP& ip, const MAC& mac, int id);

	bool renewal(const IP& ip, const IP& mask, const MAC& mac);

	bool renewal(const IP& ip, const MAC& mac);

	std::pair<IP, long long int> apply();

private:

	long long int dhcpID = 0;

	std::map<TableItem, std::pair<long long, MAC>> segments;

	std::map<TableItem, std::pair<long long, int>> tempSegments;

	IP ip;
	IP mask;

	void check();

	long long int maxCount;
	long long int nowCount = 0;

	std::pair<std::pair<IP, IP>, long long int> tryApply(const IP &ip, const IP &mask);

};


#endif //NETWORKDESIGN_DHCPTABLE_H
