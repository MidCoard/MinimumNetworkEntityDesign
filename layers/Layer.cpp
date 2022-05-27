//
// Created by 周蜀杰 on 2022/5/9.
//

#include "Layer.h"

void Layer::addLowerLayer(Layer *layer) {
	this->lowerLayers.push_back(layer);
	layer->upperLayers.push_back(this);
	std::sort(this->lowerLayers.begin(), this->lowerLayers.end(), [](Layer *a, Layer *b) {
		return a->id < b->id;
	});
}

Layer::Layer(int id) : id(id) {
}

std::string Layer::getName() {
	return this->getRawName() + std::to_string(this->id == -1 ? 0 : this->id);
}

std::vector<std::string> Layer::generateGraph(int node) {
	std::vector<std::string> result;
	dfsGenerate(node, this, &result, "", true);
	return result;
}

void
Layer::dfsGenerate(int node, Layer *layer, std::vector<std::string> *lines, const std::string& suffix, bool first) {
	if (layer->lowerLayers.size() == 1)
		dfsGenerate(node, layer->lowerLayers.at(0), lines, " " + layer->getName() + suffix, first);
	else if (layer->lowerLayers.size() > 1){
		dfsGenerate(node, layer->lowerLayers.at(0), lines, " " + layer->getName() + suffix, first);
		for (int i = 1; i < layer->lowerLayers.size(); i++)
			if (layer->lowerLayers.at(i - 1)->id != layer->lowerLayers.at(i)->id)
				dfsGenerate(node, layer->lowerLayers.at(i), lines, "", false);
	}
	else {
		if (first)
			lines->push_back(std::to_string(node + 1) + " " + layer->getName() + suffix);
		else lines->push_back(" " + layer->getName() + suffix);
	}
}

void Layer::send(Block block) {
	this->sendBlockQueue.emplace(block);
}

void Layer::receive(Block block) {
	this->receiveBlockQueue.emplace(block);
}
