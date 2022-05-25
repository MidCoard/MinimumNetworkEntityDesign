//
// Created by 周蜀杰 on 2022/5/23.
//

#include "Network.h"

void Network::addNode(NetworkEntity *entity) {
	this->nodes.push_back(entity);
	this->heads.push_back(-1);
}

// add node1 -> node2 edge i.e. PC -> SWITCH
void Network::addLink(int node1, int node2, std::pair<int, int> weight) {
	Link *link = new Link(this->heads[node1], node2, weight);
	this->links.push_back(link);
	this->heads[node1] = this->links.size() - 1;
}

Network::Network() {
	this->nodes = {};
	this->links = {};
	this->heads = {};
}

void Network::dfs(int node, std::vector<bool> &visited) {
	visited[node] = true;
	std::vector<Link *> subLinks = {};
	for (int i = this->heads[node]; i != -1; i = this->links[i]->next) {
		subLinks.push_back(this->links[i]);
		if (!visited[this->links[i]->node])
			dfs(this->links[i]->node, visited);
	}
	std::sort(subLinks.begin(), subLinks.end(), [](Link *a, Link *b) {
		return a->weight.first < b->weight.first;
	});
	std::vector<int> allocated = {};
	for (auto &link: subLinks)
		if (link->weight.first != -1)
			allocated.push_back(link->weight.first);
	int allocatedPort = 0;
	int pos = 0;
	for (auto &link: subLinks)
		if (link->weight.first == -1) {
			while (allocatedPort == allocated[pos])
				pos++;
			link->weight.first = allocatedPort;
			allocatedPort++;
		}
	std::vector<int> ids = {};
	std::transform(subLinks.begin(), subLinks.end(), std::back_inserter(ids), [](Link *a) {
		return a->weight.first;
	});
	this->nodes[node]->createLayers(node, ids);
}

void Network::build() {
	std::vector<bool> visited(this->nodes.size(), false);
	for (int i = 0; i < this->nodes.size(); i++)
		if (!visited[i])
			this->dfs(i, visited);
}

void Network::generateGraph(const std::string &filename) {
	std::vector<std::string> lines = {};
	for (auto &node: this->nodes) {
		auto l = node->generateGraph();
		lines.insert(lines.end(), l.begin(), l.end());
	}
	std::vector<bool> visited(this->nodes.size(), false);
	for (int i = 0; i < this->nodes.size(); i++)
		if (!visited[i])
			this->dfs2(i, visited);
	util::writeFile(filename, lines);
}

void Network::dfs2(int node, std::vector<bool> &visited) {
	visited[node] = true;
	for (int i = this->heads[node]; i != -1; i = this->links[i]->next) {
		if (!visited[this->links[i]->node])
			dfs2(this->links[i]->node, visited);
	}
}

Link::Link(int next, int self, std::pair<int, int> weight) {
	this->next = next;
	this->node = self;
	this->weight = weight;
}
