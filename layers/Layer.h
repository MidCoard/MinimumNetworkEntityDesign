//
// Created by 周蜀杰 on 2022/5/9.
//

#ifndef NETWORKDESIGN_LAYER_H
#define NETWORKDESIGN_LAYER_H

class NetworkEntity;

#include "string"
#include "vector"
#include "iostream"
#include "Block.h"
#include "BlockingCollection.h"

class Layer {
public:
	Layer(int id, NetworkEntity *networkEntity);

	virtual ~Layer();

	virtual std::string getRawName() = 0;

	void addLowerLayer(Layer *layer);

	std::string getName();

	std::vector<std::string> generateGraph(int node);

	virtual void send(Block *block);

	virtual void receive(int id, Block *block);

	virtual void start();

	virtual void stop();

	virtual void handleSend(Block *block) = 0;

	virtual void handleReceive(int id, Block *block) = 0;

	// the two handle methods should delete the block its created,
	// the passed block should not be deleted by current method

	void log(const std::string &message);

	void error(const std::string &message);

	void debug(const std::string &message);

	[[nodiscard]] int getID() const;

	std::vector<Layer *> lowerLayers;
    NetworkEntity *networkEntity;
protected:
	std::vector<Layer *> upperLayers;

	code_machina::BlockingQueue<Block *> sendBlockQueue;
	code_machina::BlockingQueue<std::pair<int, Block *>> receiveBlockQueue;
	bool shouldStop = false;
private:
	// this two pointer will be deleted when closing
	std::thread *sendThread = nullptr;
	std::thread *receiveThread = nullptr;
	int id;

	void
	dfsGenerate(int node, Layer *layer, std::vector<std::string> *lines, const std::string &suffix, bool first);
};


#endif //NETWORKDESIGN_LAYER_H
