//
// Created by 周蜀杰 on 2022/5/23.
//

#include "MAC.h"


MAC::MAC(std::string mac) : mac(std::move(mac)) {
	if (this->mac.size() != 17)
		throw std::invalid_argument("invalid MAC address " + this->mac);
	for (int i = 0; i < 17; i++)
		if (i % 3 == 2) {
			if (this->mac[i] != ':')
				throw std::invalid_argument("invalid MAC address " + this->mac);
		} else if (!isxdigit(this->mac[i]))
				throw std::invalid_argument("invalid MAC address " + this->mac);
}


MAC generateMAC() {
	std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> u(0, 255);
	std::string mac;
	for (int i = 0; i < 6; ++i) {
		mac += util::completeWith(util::toHex(u(e)),2,'0');
		if (i != 5)
			mac += ":";
	}
	return MAC(mac);
}