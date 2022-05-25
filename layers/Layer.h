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
	virtual std::string getName() = 0;
	void addLowerLayer(Layer *layer);
	virtual ~Layer();
protected:
	std::vector<Layer*> * lowerLayers;
	std::vector<Layer*> * upperLayers;
private:
	int id;
};


#endif //NETWORKDESIGN_LAYER_H
