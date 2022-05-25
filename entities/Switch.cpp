//
// Created by 周蜀杰 on 2022/5/21.
//

#include "Switch.h"


Switch::Switch(MAC* mac) : NetworkEntity(new LinkLayer(mac)) {}

void Switch::createLayers(std::vector<int> ids) {

}
