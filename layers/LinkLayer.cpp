#include "LinkLayer.h"

LinkLayer::LinkLayer() : LinkLayer(-1) {
}

std::string LinkLayer::getRawName() {
	return "LNK";
}


LinkLayer::LinkLayer(int id) : Layer(id) {

}

void LinkLayer::setMAC(int id, const MAC& mac) {
	idMacMap.insert({id, mac});
}

void LinkLayer::dealSend(Block *block) {

}

void LinkLayer::dealReceive(int id, Block *block) {
}
