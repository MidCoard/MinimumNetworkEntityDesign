#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include "Util.h"
#include "Layer.h"
#include "PC.h"
#include "Switch.h"
#include "ISP.h"
#include "network/IP.h"
#include "network/MAC.h"
#include "network/INetAddress.h"
#include "Network.h"
#include "DefaultRouter.h"
#include "NetworkEntity.h"
#include "BlockingCollection.h"
#include "DHCPHelper.h"

std::vector<std::string>
availableLine(std::vector<std::string>::iterator *begin, std::vector<std::string>::iterator end) {
	while (*begin != end) {
		std::string trim = util::trim(**begin);
		(*begin)++;
		if (trim.length() == 0)
			continue;
		if (util::startWith(trim, "#"))
			continue;
		return util::split(trim, " ");
	}
	return {};
}


// not necessary for the sub vector and made the process slow, however, I prefer!
NetworkEntity *createEntity(Network *network, int node, const std::string &name, std::vector<std::string> vector,
                            std::vector<std::basic_string<char>, std::allocator<std::basic_string<char>>>::iterator *begin,
                            std::vector<std::basic_string<char>, std::allocator<std::basic_string<char>>>::iterator end) {
	if (util::equalsIgnoreCase(name, "PC") || util::equalsIgnoreCase(name, "PCI")) {
		IP *ip = nullptr;
		IP *mask = nullptr;
		IP *gateway = nullptr;
		MAC *mac = nullptr;
		INetAddress *linkAddress = nullptr;
		INetAddress *physicalAddress = nullptr;
		if (!vector.empty() && vector[0] != "-")
			ip = new IP(vector[0]);
		if (vector.size() >= 2 && vector[1] != "-")
			mask = new IP(vector[1]);
		if (vector.size() >= 3 && vector[2] != "-")
			gateway = new IP(vector[2]);
		if (vector.size() >= 4 && vector[3] != "-")
			mac = new MAC(vector[3]);
		if (vector.size() >= 5 && vector[4] != "-")
			linkAddress = new INetAddress(createINetAddress(vector[4]));
		if (vector.size() >= 6 && vector[5] != "-")
			physicalAddress = new INetAddress(createINetAddress(vector[5]));
		return new PC(network, node, ip, mask, gateway, mac, linkAddress, physicalAddress);
	} else if (util::equalsIgnoreCase(name, "SWITCH") || util::equalsIgnoreCase(name, "SWITCHI")) {
		int size = -1;
		if (!vector.empty() && vector[0] != "-" && util::isNumber(vector[0]))
			size = std::stoi(vector[0]);
		std::map<int, SwitchConfiguration *> switchConfigurations;
		for (int i = 0; i < size; i++) {
			std::vector<std::string> subVector = availableLine(begin, end);
			INetAddress *linkAddress = nullptr;
			INetAddress *physicalAddress = nullptr;
			if (!subVector.empty() && subVector[0] != "-")
				linkAddress = new INetAddress(createINetAddress(subVector[0]));
			if (subVector.size() >= 2 && subVector[1] != "-")
				physicalAddress = new INetAddress(createINetAddress(subVector[1]));
			switchConfigurations.insert({i, new SwitchConfiguration(linkAddress,
			                                                        physicalAddress)});
		}
		return new Switch(network, node, switchConfigurations);
	} else if (util::equalsIgnoreCase(name, "ROUTER") || util::equalsIgnoreCase(name, "ROUTERI")) {
		int size = -1;
		if (!vector.empty() && vector[0] != "-" && util::isNumber(vector[0]))
			size = std::stoi(vector[0]);
		IP *segment0 = nullptr;
		IP *mask0 = nullptr;
		IP *gateway0 = nullptr;
		MAC *mac = nullptr;
		INetAddress *linkAddress = nullptr;
		INetAddress *physicalAddress = nullptr;
		bool flag = false;
		IP *segment = nullptr;
		IP *mask = nullptr;
		IP *gateway = nullptr;
		std::vector<std::string> lines = availableLine(begin, end);
		if (!lines.empty() && lines[0] != "-")
			segment0 = new IP(lines[0]);
		if (lines.size() >= 2 && lines[1] != "-")
			mask0 = new IP(lines[1]);
		if (lines.size() >= 3 && lines[2] != "-")
			gateway0 = new IP(lines[2]);
		if (lines.size() >= 4 && lines[3] != "-")
			mac = new MAC(lines[3]);
		if (lines.size() >= 5 && lines[4] != "-")
			linkAddress = new INetAddress(createINetAddress(lines[4]));
		if (lines.size() >= 6 && lines[5] != "-")
			physicalAddress = new INetAddress(createINetAddress(lines[5]));
		if (lines.size() >= 7 && lines[6] != "-")
			flag = util::equalsIgnoreCase(lines[6], "true");
		if (lines.size() >= 8 && lines[7] != "-")
			segment = new IP(lines[7]);
		if (lines.size() >= 9 && lines[8] != "-")
			mask = new IP(lines[8]);
		if (lines.size() >= 10 && lines[9] != "-")
			gateway = new IP(lines[9]);
		std::map<int, RouterConfiguration *> routerConfigurations;
		routerConfigurations.insert({0, new RouterConfiguration(segment0, mask0, gateway0, mac, linkAddress,
		                                                        physicalAddress)});
		for (int i = 0; i < size; i++) {
			std::vector<std::string> subVector = availableLine(begin, end);
			IP *segmenti = nullptr;
			IP *maski = nullptr;
			IP *gatewayi = nullptr;
			MAC *maci = nullptr;
			INetAddress *linkAddress0 = nullptr;
			INetAddress *physicalAddress0 = nullptr;
			if (!subVector.empty() && subVector[0] != "-")
				segmenti = new IP(subVector[0]);
			if (subVector.size() >= 2 && subVector[1] != "-")
				maski = new IP(subVector[1]);
			if (subVector.size() >= 3 && subVector[2] != "-")
				gatewayi = new IP(subVector[2]);
			if (subVector.size() >= 4 && subVector[3] != "-")
				maci = new MAC(subVector[3]);
			if (subVector.size() >= 5 && subVector[4] != "-")
				linkAddress0 = new INetAddress(createINetAddress(subVector[4]));
			if (subVector.size() >= 6 && subVector[5] != "-")
				physicalAddress0 = new INetAddress(createINetAddress(subVector[5]));
			routerConfigurations.insert({i + 1, new RouterConfiguration(
					segmenti, maski, gatewayi, maci, linkAddress0, physicalAddress0)});
		}
		if (!flag)
			return (NetworkEntity *) new DefaultRouter(network, node, routerConfigurations);
	}
	return nullptr;
}

std::pair<int, int> loadNodePort(const std::string &node) {
	if (util::startWith(node, "(") && util::endWith(node, ")")) {
		auto parts = util::split(node.substr(1, node.length() - 1), ",");
		if (parts.size() != 2)
			throw std::invalid_argument("invalid node port");
		return {std::stoi(parts[0]), std::stoi(parts[1])};
	}
	return {std::stoi(node), -1};
}

Network *loadNetwork(const std::string &networkFile, const std::string &graphFile) {
	std::vector<std::string> lines = util::readFile(networkFile);
	auto begin = lines.begin();
	auto nodesAndLinks = availableLine(&begin, lines.end());
	if (nodesAndLinks.size() != 2)
		return nullptr;
	auto *network = new Network();
	network->addNode((NetworkEntity *) new ISP(network));
	int nodes = std::stoi(nodesAndLinks[0]);
	int links = std::stoi(nodesAndLinks[1]);
	for (int i = 1; i <= nodes; i++) {
		auto entity = availableLine(&begin, lines.end());
		if (entity.empty()) {
			delete network;
			return nullptr;
		}
		std::string name = entity[0];
		NetworkEntity *entityObj = createEntity(network, i, name, util::subVector(entity, 1, entity.size()), &begin,
		                                        lines.end());
		if (entityObj == nullptr) {
			delete network;
			return nullptr;
		}
		network->addNode(entityObj);
		if (util::endWith(name, "I") || util::endWith(name, "i"))
			network->addUndirectedLink(0, i, {-1, 0});
	}
	for (int i = 0; i < links; i++) {
		auto link = availableLine(&begin, lines.end());
		if (link.size() != 2) {
			delete network;
			return nullptr;
		}
		std::pair<int, int> port = loadNodePort(link[0]);
		std::pair<int, int> port2 = loadNodePort(link[1]);
		network->addUndirectedLink(port.first, port2.first, {port.second, port2.second});
	}
	network->build(graphFile);
	return network;
}


Network *initialize(const std::string &networkFile, const std::string &graphFile) {
	Network *network = loadNetwork(networkFile, graphFile);
	if (network == nullptr) {
		std::cerr << "Network file is not valid" << std::endl;
		return nullptr;
	}
	for (auto node: network->getNodes())
		node->generateIP();
	for (auto node: network->getNodes())
		node->start();
	return network;
}


Network *initialize() {
	return initialize("network.in", "ne.txt");
}

Network *network = nullptr;

int main() {
//	util::setDebugMode(true);
//	network = initialize();
//	dhcp::start();
//	while (true) {
//		int op;
//		std::cin>>op;
//		if (op == -1) {
//			break;
//		} else if (op == 1) {
//			int node;
//			std::cin>>node;
//			std::cout<<network->getNodes()[node]->getName()<<std::endl;
//		} else if (op == 2) {
//			int node;
//			std::cin>>node;
//			std::string s;
//			std::cin>>s;
//			IP ip = IP(s);
//			auto* pc = (PC*)network->getNodes()[node];
//			pc->ping(ip);
//		} else if (op == 3) {
//			int node;
//			std::cin>>node;
//			auto* pc = (PC*)network->getNodes()[node];
//			std::cout<<pc->ip->str()<<std::endl;
//		} else if (op == 4) {
//			int node;
//			std::cin>>node;
//			auto* entity = network->getNodes()[node];
//			std::vector<IPConfiguration> ipConfiguration = entity->getIPConfiguration();
//			if (ipConfiguration.empty())
//				std::cout<< "No IP configuration"<<std::endl;
//			else std::cout<< ipConfiguration[0].getSegment()->str()<<std::endl;
//		} else if (op == 5) {
//			int node;
//			std::cin>>node;
//			auto* pc = (PC*)network->getNodes()[node];
//			pc->releaseIP();
//		} else if (op == 6) {
//			int node;
//			std::cin>>node;
//			auto* pc = (PC*)network->getNodes()[node];
//			pc->applyIP();
//		} else if (op == 7) {
//			int a, b;
//			std::cin>>a>>b;
//			PC* pc1 = (PC*)network->getNodes()[a];
//			PC* pc2 = (PC*)network->getNodes()[b];
//			auto pair = util::readBinaryFile("network.jpeg");
//			if (pair.first != nullptr) {
//				pc1->send(*pc2->ip, pair.first,pair.second);
//				delete[] pair.first;
//			} else
//				std::cerr<<"File not found"<<std::endl;
//		}
//	}
//	if (network != nullptr) {
//		// join
//		for (auto node: network->getNodes())
//			node->stop();
//	}
//	dhcp::stop();
//	delete network;


//	auto* block = new Block();
//	MAC a = generateMAC();
//	MAC b = generateMAC();
//	std::cout<<a.str()<<std::endl;
//	std::cout<<b.str()<<std::endl;
//	block->writeMAC(a);
//	block->writeMAC(b);
//	block->flip();
//	block->print();
//	auto* frame = new Frame(block);
//	delete block;
//	FrameTable table = FrameTable();
//
//	for (int i = 0;i<frame->getSize();i++) {
//		Block * block1 = frame->createBlock(i);
//		auto* ret = table.readFrame(block1);
//		if (ret != nullptr) {
//			std::cout<<ret->readMAC().str()<<std::endl;
//			std::cout<<ret->readMAC().str()<<std::endl;
//		}
//	}
	std::cout<<util::toHex('\\')<<std::endl;
	unsigned char source[12] = {0x58,0x16,0x29,0x15,0x5c,0x13,0x3f,0x4b,0x78,0x3f,0x1d,0xb6};
	auto* b = new Block();
	b->write(source, 12);
	b->flip();
	Frame * frame = new Frame(b);
	Block* a = frame->createBlock(0);
	using std::cout, std::endl;
	cout<<(a==nullptr)<<endl;
	unsigned char data[48] = {0xf8,0x70,0x38,0x47,0x43,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0xd,0x31,0xd1,0x3a,0xe,0x29,0xd6,0x80,0x3d,0x1,0xe9,0xb5,0x3,0xfc,0x5c,0x90,0x87,0x87,0xf9,0x15,0x87,0x76,0x80,0x0,0x0,0x0,0x0,0x3a,0x75,0xb0,0x60,0xc6};
	auto* block = new Block();
	block->write(data, 48);
	block->flip();
	FrameTable frameTable;
	frameTable.readFrame(block);
	frameTable.readFrame(a);
	kExecutor.stop();
	return 0;
}
