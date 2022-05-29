//
// Created by 周蜀杰 on 2022/5/27.
//

#include "IPConfiguration.h"

//use pointer to present null
IPConfiguration::IPConfiguration(IP *segment, IP *mask, IP *gateway) : segment(segment), mask(mask), gateway(gateway) {}

IP *IPConfiguration::getSegment() const {
	return this->segment;
}

IP *IPConfiguration::getMask() const {
	return this->mask;
}

IP *IPConfiguration::getGateway() const {
	return this->gateway;
}

bool IPConfiguration::isConfigurable() const {
	return this->segment != nullptr && this->mask != nullptr;
}

bool IPConfiguration::isValid() const {
	return this->segment != nullptr && this->mask != nullptr && this->gateway != nullptr;
}
