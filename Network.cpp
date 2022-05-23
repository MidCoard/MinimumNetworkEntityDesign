//
// Created by 周蜀杰 on 2022/5/23.
//

#include "Network.h"

void Network::addNode(NetworkEntity *entity) {
	this->nodes++;
	this->heads->push_back(-1);
}

// add node1 -> node2 edge i.e. PC -> SWITCH
void Network::addLink(int node1, int node2, double weight) {
	Link * link = new Link(this->heads->operator[](node1),node2,weight);
	this->links->push_back(link);
	this->heads->operator[](node1) = this->links->size() - 1;
}

Network::Network() {
	this->nodes = 0;
	this->links = new std::vector<Link*>();
	this->heads = new std::vector<int>();
}

Link::Link(int next, int self, double weight) {
	this->next = next;
	this->self = self;
	this->weight = weight;
}
