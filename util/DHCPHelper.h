//
// Created by 周蜀杰 on 2022/5/29.
//

#ifndef NETWORKDESIGN_DHCPHELPER_H
#define NETWORKDESIGN_DHCPHELPER_H

#include "NetworkLayer.h"
#include "thread"

namespace dhcp{

	extern std::thread * dhcpThread;

	extern bool shouldStop;

	extern code_machina::BlockingQueue<NetworkLayer *> layers;

	void request(NetworkLayer* layer);

	void start();

	void stop();

}


#endif //NETWORKDESIGN_DHCPHELPER_H
