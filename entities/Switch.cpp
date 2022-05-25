#include "Switch.h"


Switch::Switch(int node, MAC *mac) : NetworkEntity(node, new LinkLayer(mac)) {}

std::vector<std::string> Switch::createLayers(int node, std::vector<int> ids) {
	for (int id: ids)
		this->layer->addLowerLayer(new PhysicalLayer(id, new INetAddress(generatePhysicalAddress(node, id))));
	return this->layer->generateGraph(node);
}
