//
// Created by 周蜀杰 on 2022/5/27.
//

#include "IPConfiguration.h"

//use pointer to present null
IPConfiguration::IPConfiguration(IP *segment, IP *mask, IP *gateway) : segment(segment), mask(mask), gateway(gateway) {}

IP *IPConfiguration::getSegment() {
	return this->segment;
}

IP* IPConfiguration::getMask() {
	return this->mask;
}

IP* IPConfiguration::getGateway() {
	return this->gateway;
}

bool IPConfiguration::isConfigurable() {
	return this->segment != nullptr || this->mask != nullptr || this->gateway != nullptr;
}
