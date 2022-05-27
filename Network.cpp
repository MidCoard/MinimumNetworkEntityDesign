//
// Created by 周蜀杰 on 2022/5/23.
//

#include "Network.h"
#include "Router.h"

void Network::addNode(NetworkEntity *entity) {
	this->nodes.push_back(entity);
	this->heads.push_back(-1);
}

// add node1 -> node2 edge i.e. PC -> SWITCH
void Network::addUndirectedLink(int node1, int node2, std::pair<int, int> weight) {
	Link *link = new Link(this->heads[node1],  node2,this->links.size() + 1,  weight);
	Link *otherLink = new Link(this->heads[node2], node1, this->links.size(), {weight.second, weight.first});
	this->links.push_back(link);
	this->heads[node1] = this->links.size() - 1;
	this->links.push_back(otherLink);
	this->heads[node2] = this->links.size() - 1;
}

Network::Network() {
	this->nodes = {};
	this->links = {};
	this->heads = {};
}

void Network::dfs(int node, std::vector<bool> *visited, std::map<int, std::vector<std::string>> *all) {
	visited->at(node) = true;
	std::vector<Link *> subLinks = {};
	for (int i = this->heads[node]; i != -1; i = this->links[i]->next) {
		subLinks.push_back(this->links[i]);
		if (!visited->at(this->links[i]->node))
			dfs(this->links[i]->node, visited, all);
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
			while (pos < allocated.size() && allocatedPort == allocated[pos]) {
				pos++;
				allocatedPort++;
			}
			link->weight.first = allocatedPort;
			this->links[link->undirected]->weight.second = allocatedPort;
			allocatedPort++;
		}
	std::vector<int> ids = {};
	std::transform(subLinks.begin(), subLinks.end(), std::back_inserter(ids), [](Link *a) {
		return a->weight.first;
	});
	all->insert(std::make_pair(node, this->nodes[node]->createLayers(node, ids)));
}

void Network::build(const std::string& graphFile) {
	std::vector<std::string> lines = {};
	std::map<int, std::vector<std::string>> all;
	std::vector<bool> visited(this->nodes.size(), false);
	for (int i = 0; i < this->nodes.size(); i++)
		if (!visited[i])
			this->dfs(i, &visited, &all);
	for (int i = 0; i < this->nodes.size(); i++)
		lines.insert(lines.end(), all[i].begin(), all[i].end());
	visited = std::vector<bool>(this->links.size(), false);
	for (int i = 0; i < this->nodes.size(); i++)
		if (!visited[i])
			this->dfs2(i, &visited, &lines);
	util::writeFile(graphFile, lines);
}

void Network::dfs2(int node, std::vector<bool> *visited, std::vector<std::string> *lines) {
	std::vector<Link *> subLinks = {};
	for (int i = this->heads[node]; i != -1; i = this->links[i]->next) {
		if (!visited->at(i) && !visited->at(this->links[i]->undirected)) {
			visited->at(i) = true;
			subLinks.push_back(this->links[i]);
			dfs2(this->links[i]->node, visited, lines);
		}
	}
	for (auto &link: subLinks) {
		if (link->weight.first == -1 || link->weight.second == -1)
			throw std::invalid_argument("invalid network");
		lines->push_back(std::to_string(node + 1) + "," + std::to_string(link->weight.first) + "--"
			+ std::to_string(link->node + 1) + "," + std::to_string(link->weight.second));
	}

}

Network::~Network() {
	for (auto &link: this->links)
		delete link;
	for (auto &node: this->nodes)
		delete node;
}

std::vector<NetworkEntity *> Network::getNodes() {
	return this->nodes;
}

std::vector<Link *> Network::getLinks() {
	return this->links;
}

std::vector<int> Network::getHeads() {
	return this->heads;
}

Link::Link(int next, int self,int undirected, std::pair<int, int> weight) {
	this->next = next;
	this->node = self;
	this->weight = weight;
	this->undirected = undirected;
}
