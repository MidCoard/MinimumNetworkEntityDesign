#include "LinkLayer.h"

LinkLayer::LinkLayer(NetworkEntity* networkEntity) : LinkLayer(-1, networkEntity) {
}

std::string LinkLayer::getRawName() {
	return "LNK";
}


LinkLayer::LinkLayer(int id, NetworkEntity * networkEntity) : Layer(id, networkEntity) {

}

void LinkLayer::setMAC(int id, const MAC& mac) {
	idMacMap.insert({id, mac});
}

void LinkLayer::dealSend(Block block) {

}

void LinkLayer::dealReceive(int id, Block block) {
}
