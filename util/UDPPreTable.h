//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_UDPPRETABLE_H
#define NETWORKDESIGN_UDPPRETABLE_H

class AppLayer;

class UDPPacket;

#include "map"
#include "vector"
#include "network/IP.h"

class UDPPreTable {

public:
	explicit UDPPreTable(AppLayer *layer);

	std::pair<int, int> tryAllocate(const IP &ip, const IP &source, unsigned char *data, int len);

	void send(const IP &ip, const IP &source, int count, int target);

	bool requestResendPre(const IP &ip, int count);

	void resend(const IP &ip, const IP &source, int count, const std::vector<int> &ids);

private:
	int count = 0;
	std::map<int, std::pair<UDPPacket *, std::pair<int, long long>>> table;
	std::mutex mutex;
	AppLayer *layer;

	void check();
};


#endif //NETWORKDESIGN_UDPPRETABLE_H
