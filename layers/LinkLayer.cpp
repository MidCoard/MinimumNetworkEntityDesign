#include "LinkLayer.h"

LinkLayer::LinkLayer() : LinkLayer(-1) {
}

std::string LinkLayer::getRawName() {
	return "LNK";
}


LinkLayer::LinkLayer(int id) : Layer(id) {

}

void LinkLayer::setMAC(int id, MAC *mac) {
	macTable.insert({id, mac});
}

void LinkLayer::dealSend(Block *block) {

}

void LinkLayer::dealReceive(int id, Block *block) {

	MAC *mac = macTable.at(id);
}
