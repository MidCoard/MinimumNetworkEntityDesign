//
// Created by 周蜀杰 on 2022/5/9.
//

#include <iomanip>
#include "Layer.h"
#include "NetworkEntity.h"

void Layer::addLowerLayer(Layer *layer) {
	this->lowerLayers.push_back(layer);
	layer->upperLayers.push_back(this);
	std::sort(this->lowerLayers.begin(), this->lowerLayers.end(), [](Layer *a, Layer *b) {
		return a->id < b->id;
	});
}

Layer::Layer(int id, NetworkEntity *networkEntity) : id(id), networkEntity(networkEntity) {}

std::string Layer::getName() {
	return this->getRawName() + std::to_string(this->id);
}

std::vector<std::string> Layer::generateGraph(int node) {
	std::vector<std::string> result;
	dfsGenerate(node, this, &result, "", true);
	return result;
}

void
Layer::dfsGenerate(int node, Layer *layer, std::vector<std::string> *lines, const std::string &suffix, bool first) {
	if (layer->lowerLayers.size() == 1)
		dfsGenerate(node, layer->lowerLayers.at(0), lines, " " + layer->getName() + suffix, first);
	else if (layer->lowerLayers.size() > 1) {
		dfsGenerate(node, layer->lowerLayers.at(0), lines, " " + layer->getName() + suffix, first);
		for (int i = 1; i < layer->lowerLayers.size(); i++)
			if (layer->lowerLayers.at(i - 1)->id != layer->lowerLayers.at(i)->id)
				dfsGenerate(node, layer->lowerLayers.at(i), lines, "", false);
	} else {
		if (first)
			lines->push_back(std::to_string(node + 1) + " " + layer->getName() + suffix);
		else lines->push_back(" " + layer->getName() + suffix);
	}
}

void Layer::send(Block *block) {
	this->sendBlockQueue.emplace(block);
}

void Layer::receive(int id, Block *block) {
	this->receiveBlockQueue.emplace(std::make_pair(id, block));
}

void Layer::start() {
	for (auto layer: this->lowerLayers)
		layer->start();
	this->sendThread = new std::thread([this]() {
		while (true) {
			if (shouldStop)
				break;
			Block *block = nullptr;
			// this block is safe now
			code_machina::BlockingCollectionStatus status = this->sendBlockQueue.try_take(block,
			                                                                              std::chrono::milliseconds(1));
			if (status == code_machina::BlockingCollectionStatus::Ok) {
				this->handleSend(block);
				delete block;
			}
		}
	});
	this->receiveThread = new std::thread([this]() {
		while (true) {
			if (shouldStop)
				break;
			std::pair<int, Block *> pair;
			code_machina::BlockingCollectionStatus status = this->receiveBlockQueue.try_take(pair,
			                                                                                 std::chrono::milliseconds(
					                                                                                 1));
			if (status == code_machina::BlockingCollectionStatus::Ok) {
				this->handleReceive(pair.first, pair.second);
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
	for (auto layer: this->lowerLayers)
		layer->stop();
}

void Layer::log(const std::string &message) {
	auto now = std::chrono::system_clock::now();
	// get hour in system time zone
	auto now_c = std::chrono::system_clock::to_time_t(now);
	auto now_tm = *std::localtime(&now_c);
	std::stringstream ss;
	// print as 17:25:23.123
    ss << std::put_time(&now_tm, "%H:%M:%S.") << std::setfill('0') << std::setw(3)
       << now.time_since_epoch() / std::chrono::milliseconds(1) % 1000;
	std::string time = ss.str();
	printf("[%s]%s(%d): Layer %s: %s\n", time.c_str(), this->networkEntity->getName().c_str(),
	       this->networkEntity->getNode(),
	       this->getName().c_str(), message.c_str());
}

void Layer::error(const std::string &message) {
	auto now = std::chrono::system_clock::now();
	// get hour in system time zone
	auto now_c = std::chrono::system_clock::to_time_t(now);
	auto now_tm = *std::localtime(&now_c);
	std::stringstream ss;
	// print as 17:25:23.123
	ss << std::put_time(&now_tm, "%H:%M:%S.") << std::setfill('0') << std::setw(3)
	   << now.time_since_epoch() / std::chrono::milliseconds(1) % 1000;
	std::string time = ss.str();
	fprintf(stderr, "[%s]%s(%d): Layer %s: %s\n", time.c_str(), this->networkEntity->getName().c_str(),
	        this->networkEntity->getNode(),
	        this->getName().c_str(), message.c_str());
}

int Layer::getID() const {
	return this->id;
}

void Layer::debug(const std::string &message) {
	if (util::inDebugMode())
		this->error("[debug]" + message);
}

Layer::~Layer() = default;


