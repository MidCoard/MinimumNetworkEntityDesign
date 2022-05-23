#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include "Util.h"
#include "Layer.h"
#include "PhysicalLayer.h"
#include "LinkLayer.h"
#include "NetworkLayer.h"
#include "AppLayer.h"
#include "Network.h"
#include "entities/PC.h"
#include "entities/Switch.h"
#include "entities/Router.h"
#include "entities/Root.h"
#include "network/IP.h"
#include "network/MAC.h"

int getPort(int device, int layer, int entityId) {
	return 10000 + device * 1000 + layer * 100 + entityId;
}

std::vector<std::string> availableLine(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end) {
	while (begin != end) {
		std::string trim = util::trim(*begin);
		begin++;
		if (trim.length() == 0)
			continue;
		if (util::startWith(trim, "#"))
			continue;
		return util::split(trim, " ");
	}
	return {};
}


// not necessary for the sub vector and made the process slow, however, I prefer!
NetworkEntity *createEntity(const std::string &name, std::vector<std::string> vector) {
	if (util::equalsIgnoreCase(name,"PC") || util::equalsIgnoreCase(name, "PCD")) {
		IP * ip = nullptr;
		IP * gateway = nullptr;
		MAC * mac = nullptr;
		INetAddress * physicalAddress = nullptr;
		if (!vector.empty() && vector[0] != "-") {
			auto parts = util::split(vector[0], "(");
			ip = new IP(parts[0]);
			if (parts.size() == 2)
				gateway = new IP(vector[1].substr(0, vector[1].length() - 1));
		}
		if (vector.size() >= 2 && vector[1] != "-")
			mac = new MAC(vector[1]);
		if (vector.size() >= 3 && vector[2] != "-")
			physicalAddress = createINetAddress(vector[2]);
		return new PC(ip, gateway, mac, physicalAddress);
	} else if (util::equalsIgnoreCase(name, "SWITCH") || util::equalsIgnoreCase(name, "SWITCHD")) {
		return new Switch();
	} else if (util::equalsIgnoreCase(name, "ROUTER") || util::equalsIgnoreCase(name, "ROUTERD")) {
		return new Router();
	}
	return nullptr;
}

Network* loadNetwork(const std::string& networkFile) {
	std::vector<std::string> lines = util::readFile(networkFile);
	auto begin = lines.begin();
	auto nodesAndLinks = availableLine(begin, lines.end());
	if (nodesAndLinks.size() != 2)
		return nullptr;
	auto * network = new Network();
	network->addNode(new Root());
	int nodes = std::stoi(nodesAndLinks[0]);
	int links = std::stoi(nodesAndLinks[1]);
	for (int i = 1; i <= nodes; i++) {
		auto entity = availableLine(begin, lines.end());
		if (entity.empty()) {
			delete network;
			return nullptr;
		}
		std::string name = entity[0];
		NetworkEntity * entityObj = createEntity(name, util::subVector(entity, 1, entity.size()));
		if (entityObj == nullptr) {
			delete network;
			return nullptr;
		}
		network->addNode(entityObj);
		if (util::endWith(name, "D") || util::endWith(name, "d"))
			network->addLink(0, i, 0);
	}
	return network;
}


void initialize(const std::string& networkFile) {
	Network * network = loadNetwork(networkFile);
	if (network == nullptr) {
		std::cerr << "Network file is not valid" << std::endl;
		return;
	}
}


void initialize() {
	initialize("network.in");
}

int main() {
	initialize();
	return 0;
}
