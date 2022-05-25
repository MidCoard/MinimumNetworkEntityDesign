//
// Created by 周蜀杰 on 2022/5/9.
//

#ifndef NETWORKDESIGN_LAYER_H
#define NETWORKDESIGN_LAYER_H

#include "string"
#include "vector"

class Layer {
public:
	explicit Layer(int id);

	virtual std::string getRawName() = 0;

	void addLowerLayer(Layer *layer);

	virtual ~Layer();

	std::string getName();

	std::vector<std::string> generateGraph(int node);

protected:
	std::vector<Layer *> *lowerLayers;
	std::vector<Layer *> *upperLayers;
private:
	int id;

	void
	dfsGenerate(int node, Layer *layer, std::vector<std::string> *lines, const std::string& suffix, bool first);
};




#endif //NETWORKDESIGN_LAYER_H
