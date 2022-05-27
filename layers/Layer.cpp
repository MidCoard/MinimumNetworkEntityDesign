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
	return this->getRawName() + std::to_string( this->id);
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

void Layer::send(int id, Block * block) {
	this->sendBlockQueue.emplace(std::make_pair(id, block));
}

void Layer::receive(int id, Block * block) {
	this->receiveBlockQueue.emplace(std::make_pair(id, block));
}

void Layer::start() {
	this->sendThread = new std::thread([this]() {
		while (true) {
			if (shouldStop)
				break;
			std::pair<int,Block *> pair;
			code_machina::BlockingCollectionStatus status = this->sendBlockQueue.try_take(pair, std::chrono::milliseconds(1));
			if (status == code_machina::BlockingCollectionStatus::Ok) {
				this->dealSend(pair.first, pair.second);
				delete pair.second;
			}
		}
	});
	this->receiveThread = new std::thread([this]() {
		while (true) {
			if (shouldStop)
				break;
			std::pair<int,Block *> pair;
			code_machina::BlockingCollectionStatus status = this->receiveBlockQueue.try_take(pair, std::chrono::milliseconds(1));
			if (status == code_machina::BlockingCollectionStatus::Ok) {
				this->dealReceive(pair.first,pair.second);
				delete pair.second;
			}
		}
	});
}

// make sure this method is called before deleting
void Layer::stop() {
	if (shouldStop) {
		error("layer already stopped");
		return;
	}
	shouldStop = true;
	if (this->sendThread == nullptr || this->receiveThread == nullptr)
		error("threads should not be null");
	if (this->sendThread != nullptr) {
		this->sendThread->join();
		delete this->sendThread;
		this->sendThread = nullptr;
	}
	if (this->receiveThread != nullptr) {
		this->receiveThread->join();
		delete this->receiveThread;
		this->receiveThread = nullptr;
	}
}

void Layer::log(const std::string& message) {
	std::cout << "Layer " << this->getName() << ": " << message << std::endl;
}

void Layer::error(const std::string& message) {
	std::cerr << "Layer " << this->getName() << ": " << message << std::endl;
}

int Layer::getID() const {
	return this->id;
}

Layer::~Layer() = default;
