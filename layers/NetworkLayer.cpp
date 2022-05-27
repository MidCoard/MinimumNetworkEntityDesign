#include "NetworkLayer.h"

#include <utility>

NetworkLayer::NetworkLayer(NetworkEntity * networkEntity) : NetworkLayer(-1, networkEntity) {}

std::string NetworkLayer::getRawName() {
	return "NET";
}

NetworkLayer::NetworkLayer(int id, NetworkEntity * networkEntity) : Layer(id, networkEntity) {}

void NetworkLayer::setIPConfiguration(int id, IP *segment, IP *mask, IP *gateway) {
	configurations.insert_or_assign(id, IPConfiguration(segment, mask, gateway));
}

void NetworkLayer::dealReceive(int id, Block block) {

}

void NetworkLayer::dealSend(Block block) {
	std::vector<unsigned char> data = block->getData();
	// data copy is not necessary if isIPValid is false
	if (!isIPValid)
		return;
	if (data.size() < 4)
		throw std::invalid_argument("block should have target IP address");
	auto * sendBlock = new Block();
	IPConfiguration ipConfiguration = configurations.at(0);
	unsigned char ipHeader[4];
	ipHeader[0] = ipConfiguration.getSegment()->get(0);
	ipHeader[1] = ipConfiguration.getSegment()->get(1);
	ipHeader[2] = ipConfiguration.getSegment()->get(2);
	ipHeader[3] = ipConfiguration.getSegment()->get(3);
	sendBlock->write(ipHeader, 4);
	sendBlock->write(data.data(), data.size());
	IP ip = IP(data[0], data[1], data[2], data[3]);
	int targetID = this->routeTable.lookup(ip);
	if (targetID != -1)
		this->lowerLayers[0]->send(targetID, sendBlock);
	else {
		if ((ip & *ipConfiguration.getMask()) == (*ipConfiguration.getSegment() & *ipConfiguration.getMask())) {

		} else {

		}
	}
}
