//
// Created by 周蜀杰 on 2022/5/23.
//

#include "Network.h"

void Network::addNode(NetworkEntity *entity) {
	this->nodes.push_back(entity);
	this->heads.push_back(-1);
}

// add node1 -> node2 edge i.e. PC -> SWITCH
void Network::addLink(int node1, int node2, double weight) {
	Link * link = new Link(this->heads[node1],node2,weight);
	this->links.push_back(link);
	this->heads[node1] = this->links.size() - 1;
}

Network::Network() {
	this->nodes = {};
	this->links = {};
	this->heads = {};
}

void Network::build() {
	std::vector<bool> visited(this->nodes.size(), false);
	for (int i = 0; i < this->nodes.size(); i++) {
		if (!visited[i]) {
			this->dfs(i, visited);
		}
	}
}

Link::Link(int next, int self, double weight) {
	this->next = next;
	this->self = self;
	this->weight = weight;
}
