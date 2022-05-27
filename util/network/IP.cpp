//
// Created by 周蜀杰 on 2022/5/23.
//

#include "IP.h"

IP::IP(std::string ip) : ip(std::move(ip)) {
	auto parts = util::split(this->ip, ".");
	if (parts.size() != 4)
		throw std::invalid_argument("invalid IP address " + this->ip);
	for (int i = 0; i < 4; i++) {
		auto part = std::stoi(parts[i]);
		if (part < 0 || part > 255)
			throw std::invalid_argument("invalid IP address " + this->ip);
		this->bytes[i] = part;
	}
}

IP IP::operator&(IP ip) {
	unsigned char result[4];
	for (int i = 0; i < 4; i++)
		result[i] = this->bytes[i] & ip.bytes[i];
	return IP(result);
}

IP::IP(unsigned char bytes[]) {
	for (int i = 0; i < 4; i++)
		this->bytes[i] = bytes[i];
	this->ip = std::to_string(bytes[0]) + "." + std::to_string(bytes[1]) + "." + std::to_string(bytes[2]) + "." + std::to_string(bytes[3]);
}

bool IP::operator==(IP &ip) {
	for (int i = 0; i < 4; i++)
		if (this->bytes[i] != ip.bytes[i])
			return false;
	return true;
}

bool IP::operator!=(IP &ip) {
	return !(*this == ip);
}

std::string IP::str() {
	return this->ip;
}

IP::IP(unsigned char a, unsigned char b, unsigned char c, unsigned char d) {
	this->bytes[0] = a;
	this->bytes[1] = b;
	this->bytes[2] = c;
	this->bytes[3] = d;
	this->ip = std::to_string(a) + "." + std::to_string(b) + "." + std::to_string(c) + "." + std::to_string(d);
}

IP IP::getMask() {
	unsigned int raw = this->intValue();
	unsigned int defaultIP = 0xFFFFFFFFu;
	// find raw lowest 1
	int i = 0;
	while ((raw & (1u << i)) == 0)
		i++;
	return IP(defaultIP & ~((1u << i) - 1));
}

IP::IP(unsigned int ip) {
	this->bytes[0] = (ip >> 24) & 0xFF;
	this->bytes[1] = (ip >> 16) & 0xFF;
	this->bytes[2] = (ip >> 8) & 0xFF;
	this->bytes[3] = ip & 0xFF;
	this->ip = std::to_string(this->bytes[0]) + "." + std::to_string(this->bytes[1]) + "." + std::to_string(this->bytes[2]) + "." + std::to_string(this->bytes[3]);
}

unsigned int IP::intValue() {
	return (this->bytes[0] << 24) + (this->bytes[1] << 16) + (this->bytes[2] << 8) + this->bytes[3];
}

IP localhost = IP("127.0.0.1");