#include "LinkLayer.h"

#include <utility>

const int kFrameHeader = 0x7E;

LinkLayer::LinkLayer(INetAddress address) :address(std::move(address)),socket(address.createSocket()) {}

void LinkLayer::receive(const char *data, int len) const {

	//todo decode the data
}

void LinkLayer::send(char *data, int len) const {

	//todo encode the data
	this->socket.send(data, len);
}
