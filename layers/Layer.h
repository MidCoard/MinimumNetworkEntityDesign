//
// Created by 周蜀杰 on 2022/5/9.
//

#ifndef NETWORKDESIGN_LAYER_H
#define NETWORKDESIGN_LAYER_H

#include "string"
#include "vector"
#include "iostream"
#include "Block.h"
#include "BlockingCollection.h"

class Layer {
public:
	explicit Layer(int id);

	virtual std::string getRawName() = 0;

	void addLowerLayer(Layer *layer);

	std::string getName();

	std::vector<std::string> generateGraph(int node);

	void send(Block * block);

	void receive(Block * block);

	virtual void start();

	virtual void stop();

	virtual void dealSend(Block * block) = 0;

	virtual void dealReceive(Block * block) = 0;

	void log(const std::string& message);

	void error(const std::string& message);

protected:
	std::vector<Layer *> lowerLayers;
	std::vector<Layer *> upperLayers;

	std::condition_variable cv;
	std::mutex mutex;
	std::unique_lock<std::mutex> uniqueLock = std::unique_lock<std::mutex>(mutex);

	code_machina::BlockingQueue<Block *> sendBlockQueue;
	code_machina::BlockingQueue<Block *> receiveBlockQueue;
private:
	std::thread *sendThread = nullptr;
	std::thread *receiveThread = nullptr;
	bool shouldStop = false;
	int id;

	void
	dfsGenerate(int node, Layer *layer, std::vector<std::string> *lines, const std::string& suffix, bool first);
};




#endif //NETWORKDESIGN_LAYER_H
