//
// Created by 周蜀杰 on 2022/5/23.
//

#ifndef NETWORKDESIGN_NETWORK_H
#define NETWORKDESIGN_NETWORK_H

#include "algorithm"
#include "iostream"
#include "vector"
#include "entities/NetworkEntity.h"



class Link {

public:
	Link(int next,int self,std::pair<int,int> weight);
	int node;
	int next;
	std::pair<int,int> weight;
};

class Network {

public:
	Network();
	void addLink(int node1, int node2, std::pair<int,int> weight);
	void addNode(NetworkEntity * entity);
	void build();

private:
	std::vector<NetworkEntity *> nodes;
	std::vector<Link *>  links;
	std::vector<int> heads;

	void dfs(int node, std::vector<bool>& visited);
};

#endif //NETWORKDESIGN_NETWORK_H
