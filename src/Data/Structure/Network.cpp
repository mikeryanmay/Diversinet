/*
 * Tree.cpp
 *
 *  Created on: Apr 2, 2023
 *      Author: mike
 */

#include "Network.h"
#include "Node.h"
#include "Edge.h"

namespace Data {
namespace Structure {

Network::Network() {
}

Network::Network(std::vector<NodeSharedPtr> someNodes, std::vector<EdgeSharedPtr> someEdges) :
	nodes(someNodes), edges(someEdges) {

	// make sure we have an oldest node
	this->updateOldestNode();

	// update the tip nodes
	this->updateNodes();

}

Network::~Network() {
}

const std::vector<NodeSharedPtr>& Network::getNodes() const {
	return nodes;
}

const std::vector<EdgeSharedPtr>& Network::getEdges() const {
	return edges;
}

const NodeSharedPtr& Network::getOldestNode() const {
	return oldestNode;
}

const std::vector<NodeSharedPtr>& Network::getSampledTips() const {
	return sampledTips;
}

const size_t& Network::getNumSampledTips() const {
	return numSampledTips;
}

const size_t& Network::getNumExtinctTips() const {
	return numExtinctTips;
}

const size_t& Network::getNumHybridNodes() const {
	return numHybridNodes;
}

const std::vector<NodeSharedPtr>& Network::getExtinctTips() const {
	return extinctTips;
}

void Network::updateOldestNode() {

	// loop over nodes, finding oldest one
	oldestNode = nodes.at(0);
	for(std::vector<NodeSharedPtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ((*it)->getAge() > oldestNode->getAge()) {
			oldestNode = *it;
		}
	}

}

void Network::updateNodes() {

	// clear the containers of nodes
	hybridNodes.clear();
	sampledTips.clear();
	extinctTips.clear();

	// loop over nodes
	for(std::vector<NodeSharedPtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ( (*it)->getType() == Sample ) {
			sampledTips.push_back(*it);
		} else if ( (*it)->getType() == Extinction ) {
			extinctTips.push_back(*it);
		} else if ( (*it)->getType() == Hybrid || (*it)->getType() == HybridSpeciation ) {
			hybridNodes.push_back(*it);
		}
	}

	// update number of tips
	numHybridNodes = hybridNodes.size();
	numSampledTips = sampledTips.size();
	numExtinctTips = extinctTips.size();

}

std::string Network::getNewickString() {

	// reset visits per node
	for(std::vector<NodeSharedPtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		(*it)->resetVisits();
	}

	// start with the root
	NodeSharedPtr rootNode   = *oldestNode->getChildNodes().begin();
	EdgeSharedPtr originEdge = *rootNode->getEdgesToParents().begin();

	// recursively construct newick string
	std::string newick = rootNode->recursivelyConstructNewickString(originEdge) + ";";

	return newick;

}

} // end structure namespace
} // end network namespace
