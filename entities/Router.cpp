#include "Router.h"
#include "DHCPHelper.h"

Router::Router(Network* network, int node,std::map<int,RouterConfiguration*> routerConfigurations) : NetworkEntity(network,node, new RouterNetworkLayer(this)), routerConfigurations(std::move(routerConfigurations)) {
}

std::vector<std::string> Router::createLayers(int node, std::vector<int> ids) {
	for (int id: ids) {
		if (!this->routerConfigurations.empty()) {
			RouterConfiguration* routerConfiguration = routerConfigurations.at(id);
			((RouterNetworkLayer *) this->layer)->setIPConfiguration(id, routerConfiguration->getSegment(),
			                                                         routerConfiguration->getMask(),
			                                                         routerConfiguration->getGateway());
			auto *linkLayer = new LinkLayer(id,this);
			if (routerConfiguration->getMAC() != nullptr)
				linkLayer->setMAC(id, *routerConfiguration->getMAC());
			else
				linkLayer->setMAC(id, generateMAC());
			this->layer->addLowerLayer(linkLayer);
			auto* physicalLayer = new PhysicalLayer(id, this,routerConfiguration->getLinkAddress() == nullptr ? generateLinkAddress(node, id) : *routerConfiguration->getLinkAddress()
															 , routerConfiguration->getPhysicalAddress() == nullptr ? generatePhysicalAddress(node ,id) : *routerConfiguration->getPhysicalAddress());
			linkLayer->addLowerLayer(physicalLayer);
		} else {
			((RouterNetworkLayer *) this->layer)->setIPConfiguration(id, nullptr, nullptr, nullptr);
			auto *linkLayer = new LinkLayer(id, this);
			linkLayer->setMAC(id, generateMAC());
			this->layer->addLowerLayer(linkLayer);
			auto *physicalLayer = new PhysicalLayer(id,this, generateLinkAddress(node ,id), generatePhysicalAddress(node, id));
			linkLayer->addLowerLayer(physicalLayer);
		}
	}
	return this->layer->generateGraph(node);
}

bool Router::isRouter() {
	return true;
}

bool Router::isIPAvailable() {
	return true;
}

Router::~Router() {
	for (const auto& [key, value] : this->routerConfigurations)
		delete value;
	delete this->segment;
	delete this->mask;
	delete this->gateway;
}

void Router::start() {
	NetworkEntity::start();
	((NetworkLayer*)this->layer)->sendDHCP();
	dhcp::request((NetworkLayer*)this->layer);
}

RouterConfiguration::RouterConfiguration(IP *segment, IP *mask, IP *gateway, MAC *mac, INetAddress *linkAddress,
                                         INetAddress *physicalAddress)
		: segment(segment), mask(mask), gateway(gateway), mac(mac),linkAddress(linkAddress), physicalAddress(physicalAddress) {}

IP *RouterConfiguration::getSegment() {
	return this->segment;
}

IP * RouterConfiguration::getMask() {
	return this->mask;
}

IP * RouterConfiguration::getGateway() {
	return this->gateway;
}

MAC* RouterConfiguration::getMAC() {
	return this->mac;
}

INetAddress* RouterConfiguration::getPhysicalAddress() {
	return this->physicalAddress;
}

INetAddress *RouterConfiguration::getLinkAddress() {
	return this->linkAddress;
}

RouterConfiguration::~RouterConfiguration() {
	delete segment;
	delete mask;
	delete gateway;
	delete mac;
	delete linkAddress;
	delete physicalAddress;
}

RouterNetworkLayer::RouterNetworkLayer(NetworkEntity * networkEntity) : NetworkLayer(networkEntity) {}

RouterNetworkLayer::RouterNetworkLayer(int id, NetworkEntity * networkEntity) : NetworkLayer(id, networkEntity) {}

void RouterNetworkLayer::handleReceive(int id, Block *block) {
	routeTable.check();
	if (block->getSendCount() < 0)
		return;
	if (block->getRemaining() < 8)
		return;
	IP source = block->readIP();
	IP destination = block->readIP();
	IPConfiguration ipConfiguration = configurations.at(0);
	if (destination.isBroadcast() || (ipConfiguration.getSegment() != nullptr && destination == *ipConfiguration.getSegment())) {
		// this packet is for me or broadcast
		if (destination.isBroadcast()) {
			// broadcast
			// first check if ip valid
			if (!this->isIPValid)
				return;
			// then check if it is dhcp request or dhcp discover
			unsigned char header;
			block->read(&header, 1);
			if (header == 0x01 && source == LOCAL0) {
				// dhcp discover

			} else if (header == 0x03 && source == LOCAL0) {
				// dhcp request

			} else {
				// this is broadcast packet
				auto * newBlock = new Block();
				newBlock->writeMAC(BROADCAST_MAC);
				newBlock->write(0);
				newBlock->writeIP(source);
				newBlock->writeIP(destination);
				newBlock->write(header);
				newBlock->writeBlock(block);
				newBlock->flip();
				for (auto*layer : this->lowerLayers)
					if (layer->getID() != id)
						layer->handleReceive(id, newBlock->copy());
				delete newBlock;
			}
		} else {
			// this packet is for me
			// if I have upper layer send it to upper layer
			unsigned char header;
			block->read(&header, 1);
			switch (header) {
				default:
					if (this->upperLayers.size() == 1)
						this->upperLayers[0]->receive(this->getID(), new Block(block));
			}
		}
	} else {
		if (!this->isIPValid)
			return;
		// transfer
		std::pair<IP, int> pair = routeTable.lookup(destination);
		if (pair.second == -1)
			// do not know how to transfer
			return;
		// find the next hop
		IP nextHop = pair.first;
		// find the next hop mac
		MAC nextHopMAC = this->arpTable.lookup(nextHop);
		if (nextHopMAC.isBroadcast()) {
			((LinkLayer*)this->lowerLayers[pair.second])->sendARP(*ipConfiguration.getSegment(),nextHop);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			auto * newBlock = new Block(block->getSendCount() - 1);
			newBlock->writeIP(source);
			newBlock->writeIP(destination);
			newBlock->writeBlock(block);
			newBlock->flip();
			this->receive(id, newBlock);
		} else {
			auto *newBlock = new Block();
			newBlock->writeMAC(nextHopMAC);
			newBlock->write(0);
			newBlock->writeIP(source);
			newBlock->writeIP(destination);
			newBlock->writeBlock(block);
			newBlock->flip();
			this->lowerLayers[pair.second]->handleReceive(id, newBlock);
		}
	}
}

void RouterNetworkLayer::sendDHCP() {
	this->sendDHCP0(true);
}
