//
// Created by 周蜀杰 on 2022/5/23.
//

#include "IP.h"

IP::IP(std::string ip) : ip(std::move(ip)) {
	auto parts = util::split(this->ip, ".");
	if (parts.size() != 4)
		throw std::invalid_argument("invalid IP address");
	for (int i = 0; i < 4; i++) {
		auto part = std::stoi(parts[i]);
		if (part < 0 || part > 255)
			throw std::invalid_argument("invalid IP address");
		this->bytes[i] = part;
	}
}

IP localhost = IP("127.0.0.1");