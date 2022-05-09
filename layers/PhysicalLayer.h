//
// Created by 周蜀杰 on 2022/5/9.
//

#ifndef NETWORKDESIGN_PHYSICALLAYER_H
#define NETWORKDESIGN_PHYSICALLAYER_H

#include "Layer.h"


class PhysicalLayer : public Layer {
public:
	PhysicalLayer(int device,int entityId);
};


#endif //NETWORKDESIGN_PHYSICALLAYER_H
