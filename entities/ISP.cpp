#include "ISP.h"
#include "Network.h"

const IP *kRootIP = new IP("0.0.0.0");
const IP *kRootMask = new IP("0.0.0.0");
const IP *kRootGateway = new IP("0.0.0.1");

ISP::ISP(Network *network) : NetworkEntity(network, 0, new RootAppLayer(this)) {
}

void ISP::generateIP() {
	if (generatedIP)
		return;
	std::vector<bool> visited(this->network->getNodes().size(), false);
	visited.at(this->node) = true;
	for (int i = this->network->getHeads()[node]; i != -1; i = this->network->getLinks()[i]->next) {
		int port = this->network->getLinks()[i]->weight.first;
		int subNode = this->network->getLinks()[i]->node;
		auto child = this->network->getNodes()[subNode];
		std::vector<IPConfiguration> configurations;
		IPConfiguration ipConfiguration = networkLayer->getIPConfiguration(port);
		if (ipConfiguration.isConfigurable())
			configurations.push_back(ipConfiguration);
		if (child->isIPAvailable()) {
			visited.at(subNode) = true;
			auto configuration = child->getIPConfiguration();
			configurations.insert(configurations.end(), configuration.begin(), configuration.end());
		} else
			dfsAllocateIP(subNode, &visited, &configurations);
		IP defaultIP = IP(255, 255, 255, 255);
		IP defaultMask = IP(255, 255, 255, 255);
		IP * defaultGateway = ipConfiguration.getGateway();
		for (auto &configuration: configurations) {
			defaultMask = defaultMask & *configuration.getMask() & defaultIP.mix(*configuration.getSegment() & *configuration.getMask());
			defaultIP = defaultIP & (*configuration.getSegment() & *configuration.getMask());
			if (configuration.getGateway() != nullptr && defaultGateway == nullptr)
				defaultGateway = configuration.getGateway();
		}
		delete ipConfiguration.getSegment();
		delete ipConfiguration.getMask();
		networkLayer->setIPConfiguration(port, new IP(defaultIP), new IP(defaultMask), defaultGateway);
	}
	generatedIP = true;
}

std::vector<std::string> ISP::createLayers(int node, std::vector<int> ids) {
	this->networkLayer = new RouterNetworkLayer(this);
	this->layer->addLowerLayer(networkLayer);
	auto *emptyLinkLayer = new EmptyLinkLayer(0, this);
	this->networkLayer->addLowerLayer(emptyLinkLayer);
	auto *emptyPhysicalLayer = new EmptyPhysicalLayer(0, this);
	emptyLinkLayer->addLowerLayer(emptyPhysicalLayer);
	for (int id: ids) {
		networkLayer->setIPConfiguration(id, nullptr, nullptr,
		                                 nullptr);
		auto *linkLayer = new LinkLayer(id, this);
		linkLayer->setMAC(id, generateMAC());
		networkLayer->addLowerLayer(linkLayer);
		auto *physicalLayer = new PhysicalLayer(id, this, generateLinkAddress(node, id),
		                                        generatePhysicalAddress(node, id));
		linkLayer->addLowerLayer(physicalLayer);
	}
	return this->layer->generateGraph(node);
}

void ISP::dfsAllocateIP(int node, std::vector<bool> *visited, std::vector<IPConfiguration> *configurations) {
	visited->at(node) = true;
	for (int i = this->network->getHeads()[node]; i != -1; i = this->network->getLinks()[i]->next) {
		int subNode = this->network->getLinks()[i]->node;
		if (visited->at(subNode))
			continue;
		auto child = this->network->getNodes()[subNode];
		if (child->isIPAvailable()) {
			visited->at(subNode) = true;
			auto configuration = child->getIPConfiguration();
			configurations->insert(configurations->end(), configuration.begin(), configuration.end());
		} else
			dfsAllocateIP(subNode, visited, configurations);
	}
}

void ISP::start() {
	this->networkLayer->isIPValid = true;
	this->networkLayer->tables.push_back(new DHCPTable(*kRootIP, *kRootMask, *kRootGateway));
	for (int i = 1; i < this->networkLayer->lowerLayers.size(); i++) {
		auto *layer = (LinkLayer *) this->networkLayer->lowerLayers[i];
		IPConfiguration configuration = this->networkLayer->getIPConfiguration(i);
		if (configuration.getSegment()->isBroadcast() || configuration.getMask()->isBroadcast()) {
			std::pair<IP,IP> pair = this->networkLayer->tables[0]->applySegment();
			if (pair.first.isBroadcast())
				throw std::runtime_error("no ip segment available");
			else {
				bool flag = this->networkLayer->tables[0]->tryApply(pair.first, pair.second,layer->getMAC(), this->networkLayer->tables[0]->dhcpID++);
				if (!flag)
					throw std::runtime_error("no ip segment available");
				delete configuration.getSegment();
				delete configuration.getMask();
				delete configuration.getGateway();
				this->networkLayer->setIPConfiguration(i, new IP(pair.first), new IP(pair.second), new IP(pair.first.intValue() + 1));
				this->networkLayer->tables.push_back(new DHCPTable(pair.first, pair.second,IP(pair.first.intValue() + 1)));
			}
		}
		else {
			bool flag = this->networkLayer->tables[0]->tryApply(*configuration.getSegment(), *configuration.getMask(), layer->getMAC(), -1);
			if (!flag) {
				std::pair<IP,IP> pair = this->networkLayer->tables[0]->applySegment();
				if (pair.first.isBroadcast())
					throw std::runtime_error("no ip segment available");
				else {
					flag = this->networkLayer->tables[0]->tryApply(pair.first, pair.second,layer->getMAC(), this->networkLayer->tables[0]->dhcpID++);
					if (!flag)
						throw std::runtime_error("no ip segment available");
				}
				delete configuration.getSegment();
				delete configuration.getMask();
				delete configuration.getGateway();
				this->networkLayer->setIPConfiguration(i, new IP(pair.first), new IP(pair.second), new IP(pair.first.intValue() + 1));
				this->networkLayer->tables.push_back(new DHCPTable(pair.first, pair.second, IP(pair.first.intValue() + 1)));
			} else {
				if (configuration.getGateway() == nullptr) {
					this->networkLayer->setIPConfiguration(i, configuration.getSegment(), configuration.getMask(),
					                                       new IP(configuration.getSegment()->intValue() + 1));
					this->networkLayer->tables.push_back(
							new DHCPTable(*configuration.getSegment(), *configuration.getMask(), IP(configuration.getSegment()->intValue() + 1)));
				} else this->networkLayer->tables.push_back(new DHCPTable(*configuration.getSegment(), *configuration.getMask(),
				                                                           *configuration.getGateway()));
			}
		}
	}
	NetworkEntity::start();
}

std::string ISP::getName() {
	return "ISP";
}

RootAppLayer::RootAppLayer(NetworkEntity *networkEntity) : AppLayer(networkEntity) {
}

void EmptyLinkLayer::start() {}

void EmptyLinkLayer::stop() {}

EmptyLinkLayer::EmptyLinkLayer(int id, NetworkEntity *networkEntity) : LinkLayer(id, networkEntity) {}

void EmptyLinkLayer::handleReceive(int id, Block *block) {}

void EmptyLinkLayer::handleSend(Block *block) {}

void EmptyPhysicalLayer::start() {}

void EmptyPhysicalLayer::stop() {}

void EmptyPhysicalLayer::handleReceive(int id, Block *block) {}

void EmptyPhysicalLayer::handleSend(Block *block) {}

EmptyPhysicalLayer::EmptyPhysicalLayer(int id, NetworkEntity *networkEntity) : PhysicalLayer(id, networkEntity,
                                                                                             INetAddress(LOCALHOST, 0),
                                                                                             INetAddress(LOCALHOST,
                                                                                                         0)) {}
