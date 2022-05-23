#include "NetworkEntity.h"

#include <utility>

NetworkEntity::NetworkEntity(MAC mac, INetAddress physicalAddress)  :mac(std::move(mac)), physicalAddress(std::move(physicalAddress)) {
}
