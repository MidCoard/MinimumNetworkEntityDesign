#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include "Util.h"
#include "Layer.h"
#include "PhysicalLayer.h"
#include "AppLayer.h"
#include "PC.h"
#include "Switch.h"
#include "Router.h"
#include "Root.h"
#include "network/IP.h"
#include "network/MAC.h"
#include "Network.h"

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
	NetworkEntity *entity = nullptr;
	if (util::equalsIgnoreCase(name,"PC") || util::equalsIgnoreCase(name, "PCD")) {
		IP* ip = nullptr;
		IP* gateway = nullptr;
		MAC* mac = nullptr;
		INetAddress* physicalAddress = nullptr;
		if (!vector.empty() && vector[0] != "-") {
			auto parts = util::split(vector[0], "(");
			ip = new IP(parts[0]);
			if (parts.size() == 2)
				gateway = new IP(vector[1].substr(0, vector[1].length() - 1));
		}
		if (vector.size() >= 2 && vector[1] != "-")
			mac = new MAC(vector[1]);
		if (vector.size() >= 3 && vector[2] != "-")
			physicalAddress = new INetAddress(createINetAddress(vector[2]));
		entity = new PC(ip, gateway, mac, physicalAddress);
	} else if (util::equalsIgnoreCase(name, "SWITCH") || util::equalsIgnoreCase(name, "SWITCHD")) {
		MAC* mac = nullptr;
		if (!vector.empty() && vector[0] != "-")
			mac = new MAC(vector[0]);
		return new Switch(mac);
	} else if (util::equalsIgnoreCase(name, "ROUTER") || util::equalsIgnoreCase(name, "ROUTERD")) {
		IP* segment = nullptr;
		IP* mask = nullptr;
		if (!vector.empty() && vector[0] != "-")
			segment = new IP(vector[0]);
		if (vector.size() >= 2 && vector[1] != "-")
			mask = new IP(vector[1]);
		return new Router(segment, mask);
	}
	return entity;
}

std::pair<int,int> loadNodePort(const std::string& node) {
	if (util::startWith(node, "(") && util::endWith(node, ")")) {
		auto parts = util::split(node.substr(1,node.length() - 1),",");
		if (parts.size() != 2)
			throw std::invalid_argument("invalid node port");
		return {std::stoi(parts[0]),std::stoi(parts[1])};
	}
	return {std::stoi(node),-1};
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
		if (util::endWith(name, "D") || util::endWith(name, "d")) {
			network->addLink(0, i, {-1, -1});
			network->addLink(i, 0, {-1, -1});
		}
	}
	for (int i = 0;i < links; i++) {
		auto link = availableLine(begin, lines.end());
		if (link.size() != 2) {
			delete network;
			return nullptr;
		}
		std::pair<int,int> port = loadNodePort(link[0]);
		std::pair<int,int> port2 = loadNodePort(link[1]);
		network->addLink(port.first, port2.first, {port.second,port2.second});
		network->addLink(port2.first, port.first, {port2.second,port.second});
	}
	network->build();
	return network;
}


void initialize(const std::string& networkFile) {
	Network * network = loadNetwork(networkFile);
	// why ?
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
