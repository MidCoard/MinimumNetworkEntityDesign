//
// Created by 周蜀杰 on 2022/5/23.
//

#include <thread>
#include "MAC.h"
#include "chrono"


MAC::MAC(std::string mac) : mac(std::move(mac)) {
	if (this->mac.size() != 17)
		throw std::invalid_argument("invalid MAC address " + this->mac);
	std::vector<std::string> macs = util::split(this->mac, ":");
	if (macs.size() != 6)
		throw std::invalid_argument("invalid MAC address " + this->mac);
	for (int i = 0; i < 6; i++) {
		this->bytes[i] = std::stoi(macs[i], nullptr, 16);
	}
}

MAC::MAC(const unsigned char *bytes) {
	for (int i = 0; i < 6; i++) {
		this->bytes[i] = bytes[i];
	}
	this->mac =
			util::completeWith(util::toHex(bytes[0]), 2, '0') + ":" +
			util::completeWith(util::toHex(bytes[1]), 2, '0') + ":" +
			util::completeWith(util::toHex(bytes[2]), 2, '0') + ":" +
			util::completeWith(util::toHex(bytes[3]), 2, '0') + ":" +
			util::completeWith(util::toHex(bytes[4]), 2, '0') + ":" + util::completeWith(util::toHex(bytes[5]), 2, '0');
}

MAC::MAC(unsigned char i, unsigned char i1, unsigned char i2, unsigned char i3, unsigned char i4, unsigned char i5) {
	this->bytes[0] = i;
	this->bytes[1] = i1;
	this->bytes[2] = i2;
	this->bytes[3] = i3;
	this->bytes[4] = i4;
	this->bytes[5] = i5;
	this->mac =
			util::completeWith(util::toHex(bytes[0]), 2, '0') + ":" +
			util::completeWith(util::toHex(bytes[1]), 2, '0') + ":" +
			util::completeWith(util::toHex(bytes[2]), 2, '0') + ":" +
			util::completeWith(util::toHex(bytes[3]), 2, '0') + ":" +
			util::completeWith(util::toHex(bytes[4]), 2, '0') + ":" + util::completeWith(util::toHex(bytes[5]), 2, '0');
}

bool MAC::operator<(const MAC &mac) const {
	for (int i = 0; i < 6; i++) {
		if (this->bytes[i] < mac.bytes[i])
			return true;
		else if (this->bytes[i] > mac.bytes[i])
			return false;
	}
	return false;
}

bool MAC::operator==(const MAC &mac) const {
	for (int i = 0; i < 6; i++) {
		if (this->bytes[i] != mac.bytes[i])
			return false;
	}
	return true;
}

unsigned char MAC::get(int index) const {
	return this->bytes[index];
}

bool MAC::isBroadcast() const {
	for (int i = 0; i < 6; i++)
		if (this->bytes[i] != 0xff)
			return false;
	return true;
}

std::string MAC::str() {
	return this->mac;
}

bool MAC::operator!=(const MAC &mac) const {
	return !(this->operator==(mac));
}

MAC generateMAC() {
	std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
	std::uniform_int_distribution<int> u(0, 255);
	std::string mac;
	for (int i = 0; i < 6; ++i) {
		mac += util::completeWith(util::toHex(u(e)), 2, '0');
		if (i != 5)
			mac += ":";
	}
	return MAC(mac);
}

MAC BROADCAST_MAC = MAC(0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu);