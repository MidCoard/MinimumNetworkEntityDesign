#include "LinkLayer.h"

#include <utility>

LinkLayer::LinkLayer(INetAddress address) :address(std::move(address)),socket(address.createSocket()) {}

void LinkLayer::receive(const char *data, int len) const {

	//todo decode the data
}

void LinkLayer::send(char *data, int len) const {

	//todo encode the data
	this->socket.send(data, len);
}
