#include <iostream>
#include <vector>
#include <fstream>
#include "Util.h"
#include "Layer.h"
#include "PhysicalLayer.h"
#include "LinkLayer.h"
#include "NetworkLayer.h"
#include "AppLayer.h"

int getPort(int device, int layer, int entityId) {
	return 10000 + device * 1000 + layer * 100 + entityId;
}

void initialize(std::string neFileName) {
	auto lines = util::readFile(neFileName);
	int device = -1;
	Layer* upperLayer[4] = { nullptr, nullptr, nullptr, nullptr };
	for (auto line : lines) {
		auto trim = util::trim(line);
		if (trim.empty())
			continue;
		if (util::startWith(trim, "#"))
			continue;
		if (util::startWith(trim, "-"))
			continue;
		if (util::contains(trim,","))
			continue;
		// we don't need the topology
		if (util::contains(trim, "=")) {
			auto parts= util::split(trim, "=");
		} else {
			auto parts = util::split(trim, " ");
			if (parts.size() < 2)
				continue;
			device = util::isNumber(parts[0]) ? std::stoi(parts[0]) : device;
			if (device == -1)
				continue;
			for (int i = 1; i < parts.size(); i++) {
				auto part = parts[i];
				std::string host = "127.0.0.1";
				if (util::contains(part,"@")) {
					auto args = util::split(part, "@");
					if (args.size() != 2) {
						std::cerr << "Ignore " << part << std::endl;
						continue;
					}
					host = args[1];
				}
				auto name = part.substr(0,3);
				auto entityId = std::stoi(part.substr(3));
				if (name == "PHY") {
					upperLayer[i-1] = new PhysicalLayer(device, entityId, new INetAddress(host, 0));
				} else if (name == "LNK") {
					upperLayer[i-1] = new LinkLayer(device, entityId, new INetAddress(host, 0));
				} else if (name == "NET") {
					upperLayer[i-1] = new NetworkLayer(device, entityId, host);
				} else if (name == "APP") {
					upperLayer[i-1] = new AppLayer(device, entityId, host);
				} else {
					std::cerr << "Ignore " << part << std::endl;
					continue;
				}
			}
			for (int i = 1; i < 4; i++) {
				if (upperLayer[i] != nullptr)
					upperLayer[i-1]->setLowerLayer(upperLayer[i]);

			}
		}
	}
}

int main() {
	initialize("ne.txt");
	return 0;
}
