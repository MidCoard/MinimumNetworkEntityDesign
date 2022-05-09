//
// Created by 周蜀杰 on 2022/5/9.
//

#ifndef NETWORKDESIGN_LAYER_H
#define NETWORKDESIGN_LAYER_H


class Layer {
public:
	Layer(int device,int entityId);

private:
	const int device;
	const int entityId;
};


#endif //NETWORKDESIGN_LAYER_H
