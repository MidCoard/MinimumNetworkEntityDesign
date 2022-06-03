//
// Created by 周蜀杰 on 2022/5/23.
//

#ifndef NETWORKDESIGN_MAC_H
#define NETWORKDESIGN_MAC_H

#include "string"
#include <utility>
#include "random"
#include "Util.h"

class MAC {
public:
	explicit MAC(std::string mac);

	explicit MAC(const unsigned char *bytes);

	bool operator<(const MAC &mac) const;

	bool operator==(const MAC &mac) const;

	bool operator!=(const MAC &mac) const;

	[[nodiscard]] unsigned char get(int index) const;

	[[nodiscard]] bool isBroadcast() const;

	MAC(unsigned char i, unsigned char i1, unsigned char i2, unsigned char i3, unsigned char i4, unsigned char i5);

	std::string str();

private:
	std::string mac;
	unsigned char bytes[6]{};
};

MAC generateMAC();


extern MAC BROADCAST_MAC;


#endif //NETWORKDESIGN_MAC_H
