#include "NetworkEntity.h"

NetworkEntity::NetworkEntity(MAC* mac, INetAddress* physicalAddress)  :mac(mac), physicalAddress(physicalAddress) {
}

NetworkEntity::~NetworkEntity() {
	delete mac;
	delete physicalAddress;
}
