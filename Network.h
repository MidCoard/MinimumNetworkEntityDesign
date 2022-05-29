#ifndef NETWORKDESIGN_NETWORK_H
#define NETWORKDESIGN_NETWORK_H

#include "NetworkEntity.h"
#include "algorithm"
#include "iostream"
#include "vector"
#include "map"

class Link {

public:
	Link(int next, int self, int undirected, std::pair<int, int> weight);

	int undirected;
	int node;
	int next;
	std::pair<int, int> weight;
};

class Network {

public:
	Network();

	void addUndirectedLink(int node1, int node2, std::pair<int, int> weight);

	void addNode(NetworkEntity *entity);

	void build(const std::string &graphFile);

	~Network();

	std::vector<NetworkEntity *> getNodes();

	std::vector<Link *> getLinks();

	std::vector<int> getHeads();

private:
	std::vector<NetworkEntity *> nodes;
	std::vector<Link *> links;
	std::vector<int> heads;

	void dfs(int node, std::vector<bool> *visited, std::map<int, std::vector<std::string>> *all);

	void dfs2(int node, std::vector<bool> *visited, std::vector<std::string> *lines);
};

#endif //NETWORKDESIGN_NETWORK_H
