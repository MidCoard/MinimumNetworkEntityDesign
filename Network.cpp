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

void Network::dfs(int node, std::vector<bool> *visited, std::vector<std::string> * lines) {
	visited->at(node) = true;
	std::vector<Link *> subLinks = {};
	for (int i = this->heads[node]; i != -1; i = this->links[i]->next) {
		subLinks.push_back(this->links[i]);
		if (!visited->at(this->links[i]->node))
			dfs(this->links[i]->node, visited, lines);
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
	auto l = this->nodes[node]->createLayers(node, ids);
	lines->insert(lines->end(), l.begin(), l.end());
}

void Network::build(const std::string& graphFile) {
	std::vector<std::string> lines = {};
	std::vector<bool> visited(this->nodes.size(), false);
	for (int i = 0; i < this->nodes.size(); i++)
		if (!visited[i])
			this->dfs(i, &visited, &lines);
	visited = std::vector<bool>(this->nodes.size(), false);
	for (int i = 0; i < this->nodes.size(); i++)
		if (!visited[i])
			this->dfs2(i, &visited, &lines);
	util::writeFile(graphFile, lines);
}

void Network::dfs2(int node, std::vector<bool> *visited, std::vector<std::string> *lines) {
	visited->at(node) = true;
	std::vector<Link *> subLinks = {};
	for (int i = this->heads[node]; i != -1; i = this->links[i]->next) {
		subLinks.push_back(this->links[i]);
		if (!visited->at(this->links[i]->node))
			dfs2(this->links[i]->node, visited, lines);
	}
	for (auto &link: subLinks) {
		if (link->weight.first == -1 || link->weight.second == -1)
			throw std::invalid_argument("invalid network");
		lines->push_back(std::to_string(node) + "," + std::to_string(link->weight.first) + "--"
			+ std::to_string(link->node) + "," + std::to_string(link->weight.second));
	}

}

Link::Link(int next, int self, std::pair<int, int> weight) {
	this->next = next;
	this->node = self;
	this->weight = weight;
}
