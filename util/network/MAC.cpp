//
// Created by 周蜀杰 on 2022/5/23.
//

#include "MAC.h"


MAC::MAC(std::string mac) :mac(std::move(mac)) {

}


MAC generateMAC() {
	std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> u(0, 255);
	std::string mac;
	for (int i = 0; i < 6; ++i) {
		mac += std::to_string(u(e));
		if (i != 5)
			mac += ":";
	}
	return MAC(mac);
}