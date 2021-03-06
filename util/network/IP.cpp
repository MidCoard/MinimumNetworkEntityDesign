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

IP IP::operator&(IP ip) const {
	unsigned char result[4];
	for (int i = 0; i < 4; i++)
		result[i] = this->bytes[i] & ip.bytes[i];
	return IP(result);
}

IP::IP(unsigned char bytes[]) {
	for (int i = 0; i < 4; i++)
		this->bytes[i] = bytes[i];
	this->ip = std::to_string(bytes[0]) + "." + std::to_string(bytes[1]) + "." + std::to_string(bytes[2]) + "." +
	           std::to_string(bytes[3]);
}

bool IP::operator==(IP ip) const {
	for (int i = 0; i < 4; i++)
		if (this->bytes[i] != ip.bytes[i])
			return false;
	return true;
}

bool IP::operator!=(IP ip) const {
	return !(*this == ip);
}

std::string IP::str() const {
	return this->ip;
}

IP::IP(unsigned char a, unsigned char b, unsigned char c, unsigned char d) {
	this->bytes[0] = a;
	this->bytes[1] = b;
	this->bytes[2] = c;
	this->bytes[3] = d;
	this->ip = std::to_string(a) + "." + std::to_string(b) + "." + std::to_string(c) + "." + std::to_string(d);
}

IP IP::getMask() const {
	unsigned int raw = this->intValue();
	unsigned int defaultIP = 0xFFFFFFFFu;
	// find raw lowest 1
	int i = 0;
	while ((raw & (1u << i)) == 0 && i < 32)
		i++;
	return IP(defaultIP & ~((1u << i) - 1));
}

IP::IP(unsigned int ip) {
	this->bytes[0] = (ip >> 24) & 0xFF;
	this->bytes[1] = (ip >> 16) & 0xFF;
	this->bytes[2] = (ip >> 8) & 0xFF;
	this->bytes[3] = ip & 0xFF;
	this->ip = std::to_string(this->bytes[0]) + "." + std::to_string(this->bytes[1]) + "." +
	           std::to_string(this->bytes[2]) + "." + std::to_string(this->bytes[3]);
}

unsigned int IP::intValue() const {
	return (this->bytes[0] << 24) + (this->bytes[1] << 16) + (this->bytes[2] << 8) + this->bytes[3];
}

unsigned char IP::get(int index) const {
	return this->bytes[index];
}

bool IP::isInSameNetwork(const IP &ip, const IP &mask) const {
	return (*this & mask) == (ip & mask);
}

bool IP::operator<(const IP &ip) const {
	return this->intValue() < ip.intValue();
}

bool IP::isBroadcast() const {
	for (int i = 0; i < 4; i++)
		if (this->bytes[i] != 0xFF)
			return false;
	return true;
}

bool IP::operator>(const IP &ip) const {
	return this->intValue() > ip.intValue();
}

int IP::getRightZero() const {
	unsigned int raw = intValue();
	int i = 0;
	while (i < 32 && (raw & (1u << i)) == 0)
		i++;
	return i;
}

IP IP::mix(const IP &ip) {
	// find their first different bit from right
	unsigned int a = this->intValue();
	unsigned int b = ip.intValue();
	if (a == b)
		return BROADCAST_IP;
	int i = 0;
	while ((a & (1u << i)) == (b & (1u << i)) && i < 32)
		i++;
	// return mask
	return IP((~((1u << i) - 1)) << 1);
}

bool IP::isInPrivateNetwork() const {
	return (*this & PRIVATE_NETWORK_MASK) == PRIVATE_NETWORK_IP;
}

IP LOCALHOST = IP("127.0.0.1");
IP LOCAL0 = IP(0u);
IP BROADCAST_IP = IP(0xffu, 0xffu, 0xffu, 0xffu);
IP PRIVATE_NETWORK_IP = IP(192,168,0,0);
IP PRIVATE_NETWORK_MASK = IP(255,255,0,0);