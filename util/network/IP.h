//
// Created by 周蜀杰 on 2022/5/23.
//

#ifndef NETWORKDESIGN_IP_H
#define NETWORKDESIGN_IP_H

#include "string"
#include "Util.h"
#include <utility>

class IP {
public:

	explicit IP(std::string ip);

	explicit IP(unsigned char bytes[]);

	explicit IP(unsigned int ip);

	IP(unsigned char a, unsigned char b, unsigned char c, unsigned char d);

	IP operator&(IP ip) const;

	bool operator<(const IP &ip) const;

	bool operator>(const IP &ip) const;

	bool operator==(IP ip) const;

	bool operator!=(IP &ip) const;

	[[nodiscard]] bool isBroadcast() const;

	[[nodiscard]] std::string str() const;

	[[nodiscard]] unsigned int intValue() const;

	[[nodiscard]] IP getMask() const;

	[[nodiscard]] unsigned char get(int index) const;

	[[nodiscard]] bool isInSameNetwork(const IP &ip, const IP &mask) const;

	[[nodiscard]] int getRightZero() const;

	IP mix(const IP& ip);

private:
	std::string ip;
	unsigned char bytes[4]{};
};

extern IP LOCALHOST;

extern IP LOCAL0;

extern IP BROADCAST_IP;


#endif //NETWORKDESIGN_IP_H
