//
// Created by 周蜀杰 on 2022/5/9.
//

#include "Layer.h"

void Layer::addLowerLayer(Layer * layer) {
	this->lowerLayers->push_back(layer);
	layer->upperLayers->push_back(this);
}

Layer::Layer() {
	this->lowerLayers = new std::vector<Layer*>();
	this->upperLayers = new std::vector<Layer*>();
}

Layer::~Layer() {
	delete this->lowerLayers;
	delete this->upperLayers;
}
