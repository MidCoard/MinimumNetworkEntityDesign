//
// Created by 周蜀杰 on 2022/5/27.
//

#ifndef NETWORKDESIGN_IPCONFIGURATION_H
#define NETWORKDESIGN_IPCONFIGURATION_H

#include "network/IP.h"

class IPConfiguration {
public:
	IPConfiguration(IP *segment, IP *mask, IP *gateway);

	IP *getSegment();

	IP *getMask();

	IP *getGateway();

	bool isConfigurable();

	bool isValid();

private:
	IP *segment;
	IP *mask;
	IP *gateway;
};


#endif //NETWORKDESIGN_IPCONFIGURATION_H
