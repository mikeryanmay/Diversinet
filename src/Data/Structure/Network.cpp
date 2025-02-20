/*
 * Tree.cpp
 *
 *  Created on: Apr 2, 2023
 *      Author: mike
 */

#include <iostream>
#include <algorithm>

#include <boost/make_shared.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include "Network.h"
#include "Node.h"
#include "Edge.h"

#include "../Reader/IncPhyloReader.h"

namespace Data {
namespace Structure {

Network::Network(std::vector<NodeSharedPtr> someNodes, std::vector<EdgeSharedPtr> someEdges) :
	nodes(someNodes), edges(someEdges) {

	// make sure we have an oldest node
	this->updateOldestNode();

	// update the tip nodes
	this->updateNodes();

}

Network::Network(Data::NewickReader::NewickParserSharedPtr aNewickParser) {
	this->buildNetworkFromNewick(&aNewickParser->getRoot());
}

Network::~Network() {
	// only the network should own shared pointers to nodes and edges
}


void Network::buildNetworkFromNewick(const Data::NewickReader::TreeNode* aNewickRoot) {

	// make the network
	NodeSharedPtr origin = boost::make_shared<Node>(0, 0.0, Origin);

	// get the root edge length
	double originTimeRespToRoot = aNewickRoot->getLength();

	// get the root node
	size_t nodeId = aNewickRoot->defineRBCompatibleNodeID();
	NodeSharedPtr root = boost::make_shared<Node>(nodeId, originTimeRespToRoot, Root);

	// add the label
	root->setLabel(aNewickRoot->getName());

	// create the root edge
	EdgeSharedPtr rootEdge = boost::make_shared<Edge>(origin, root);

	// attach the edge
	origin->addEdge(rootEdge);
	root->addEdge(rootEdge);

	// add the nodes and edges
	nodes.push_back(origin);
	nodes.push_back(root);
	edges.push_back(rootEdge);

	// build nodes recursively
	double maxTempAge = createRecursiveNewick(aNewickRoot, root);

	// reverse age
	for(size_t iN = 0; iN < nodes.size(); ++iN) {

		double oldAge = nodes[iN]->getAge();
		double newAge = maxTempAge - oldAge;

		if(newAge < 1.e-4) {
			newAge = 0.0;
		}

		nodes[iN]->setAge(newAge);

	}

	// find all duplicate nodes
	std::vector<std::string> labels;
	for(size_t iN = 0; iN < nodes.size(); ++iN) {
		std::string thisLabel = nodes[iN]->getLabel();
		if ( thisLabel != "" ) {
			labels.push_back(thisLabel);
		}
	}

	// find labels that appear twice
	sort(labels.begin(), labels.end());
	std::vector<std::string> hybidLabels;
	for(size_t iL = 0; iL < labels.size(); ++iL) {
		if ( labels[iL] == labels[iL+1] ) {
			hybidLabels.push_back(labels[iL]);
			++iL;
		}
	}

	// merge nodes by label
	for(size_t iH = 0; iH < hybidLabels.size(); ++iH) {
		this->mergeHybridNodesMyLabels( hybidLabels[iH] );
	}

	// update the nodes
	this->updateOldestNode();
	this->ensureSimultaneousEvents();
	this->updateNodes();

}

void Network::mergeHybridNodesMyLabels(std::string aLabel) {

	// find the nodes with these labels
	std::vector<NodeSharedPtr> hybridNodes;
	for(std::vector<NodeSharedPtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ( (*it)->getLabel() == aLabel ) {
			hybridNodes.push_back(*it);
		}
	}

	// make sure we found exactly two nodes with this label
	assert( hybridNodes.size() == 2 && "Provided label is not repeated among nodes, must not be a hybrid.");

	// one of these nodes should have zero descendants
	NodeSharedPtr nodeToDrop;
	NodeSharedPtr nodeToKeep;
	if ( hybridNodes.at(0)->getChildNodes().size() == 0 ) {
		nodeToDrop = hybridNodes.at(0);
		nodeToKeep = hybridNodes.at(1);
	} else if ( hybridNodes.at(1)->getChildNodes().size() == 0 ) {
		nodeToDrop = hybridNodes.at(1);
		nodeToKeep = hybridNodes.at(0);
	} else {
		assert(false && "At least one of the candidate nodes to merge must have zero descendants");
	}

	// get the parent edge for the node we are dropping
	std::vector<EdgeSharedPtr> parentEdges = nodeToDrop->getEdgesToParents();
	assert(parentEdges.size() == 1 && "The node to drop should only have one parent");
	EdgeSharedPtr parentEdge = parentEdges.at(0);

	// assign the node to keep as the child of this edge
	parentEdge->setChild(nodeToKeep);

	// set the type of the edge
	parentEdge->setType(Hybridization);

	// add the edge to the node we are keeping
	nodeToKeep->addEdge(parentEdge);

	// set the hybrid node types
	size_t hashPos = aLabel.find("#");
	assert(hashPos != std::string::npos && "Unable to find hash symbol.");

	if ( aLabel[hashPos + 1] == 'H' ) {
		nodeToKeep->setType(HybridSpecies);
	} else if (aLabel[hashPos + 1] == 'P') {
		nodeToKeep->setType(Allopolyploid);
	} else {
		// node just has a one edge pointing into it
		nodeToKeep->setType(Hybrid);
	}

	// remove the dangling node
	for(std::vector<NodeSharedPtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ( (*it) == nodeToDrop ) {
			nodes.erase(it); // @suppress("Invalid arguments")
			break;
		}
	}

}

double Network::createRecursiveNewick(const NewickReader::TreeNode *newickNode, NodeSharedPtr treeNode) {

	assert(treeNode && "tree node invalid");

	// get age of parent node
	double maxAge = treeNode->getAge();

	// For each child of newick Node, we create the new treeNode
	for(size_t iC = 0; iC < newickNode->getChildren().size(); ++iC) {

		// get the newick child
		const NewickReader::TreeNode *childNewick = &newickNode->getChildren()[iC];

		// get the branch length of newick child
		double branchLengthToParent = childNewick->getLength();

		// get the age
		double temporaryAge = treeNode->getAge() + branchLengthToParent;

		// determine whether node is a tip or sampled ancestor
		NodeType type = Speciation;
		if ( childNewick->isLeaf() ) {
			type = Sample;
		} else if ( childNewick->isSampledAncestor() ) {
			type = SampledAncestor;
		}

		// create the node
		size_t nodeId = childNewick->defineRBCompatibleNodeID();
		NodeSharedPtr childNode = boost::make_shared<Node>(nodeId, temporaryAge, type);

		// add the label
		childNode->setLabel(childNewick->getName());

		// call recursively
		double tempMaxAge = createRecursiveNewick(childNewick, childNode);
		maxAge = std::max(tempMaxAge, maxAge);

		// create the edge
		EdgeSharedPtr newEdge = boost::make_shared<Edge>(treeNode, childNode);
		treeNode->addEdge(newEdge);
		childNode->addEdge(newEdge);

		// add nodes and edges to network structure
		nodes.push_back(childNode);
		edges.push_back(newEdge);

	}

	return maxAge;

}


void Network::ensureSimultaneousEvents(double precision) {

	// reset visits per node
	for(std::vector<NodeSharedPtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		(*it)->resetVisits();
	}

	// get the oldest node
	NodeSharedPtr originNode = this->getOldestNode();

	// call recursive function on oldest node
	this->ensureSimultaneousEventsRecursive(originNode, precision);

}

void Network::ensureSimultaneousEventsRecursive(NodeSharedPtr aNode, double precision) {

	// never re-age a sample
	if ( aNode->getType() == Sample ) {
		return;
	}

	// stop if we have already visited
	if ( aNode->visits > 0 ) {
		return;
	}

	// increment the visits
	aNode->visits++;

	// get the age of this node
	double thisNodeAge = aNode->getAge();

	// get all the children
	std::vector<NodeSharedPtr> children = aNode->getChildNodes();
	for(std::vector<NodeSharedPtr>::iterator it = children.begin(); it != children.end(); ++it) {

		// if the difference between ages is lower than 1e-precision, reset descendant to this age
		if ( fabs((*it)->getAge() - thisNodeAge) < precision ) {
			// reset age of descendant
			(*it)->setAge(thisNodeAge);
		}

		// call recursively
		this->ensureSimultaneousEventsRecursive(*it, precision);

	}

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
		} else if ( (*it)->getType() == Hybrid || (*it)->getType() == HybridSpecies || (*it)->getType() == Allopolyploid ) {
			hybridNodes.push_back(*it);
		}
	}

	// update number of tips
	numHybridNodes   = hybridNodes.size();
	numSampledTips   = sampledTips.size();
	numExtinctTips   = extinctTips.size();

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

void Network::pruneExtinctTips() {

	// reset visits per node
	for(std::vector<NodeSharedPtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		(*it)->resetVisits();
	}

	// get the oldest node
	NodeSharedPtr originNode = this->getOldestNode();

	// call recursive function on oldest node
	this->pruneExtinctTipsRecursive(originNode);

	// update the nodes vectors
	this->updateNodes();

	return;

}

void Network::pruneExtinctTipsRecursive(NodeSharedPtr aNode) {

	// if this is a sample, do nothing
	if ( aNode->getType() == Sample ) {
		return;
	}

	// stop if we have already visited
	if ( aNode->visits > 0 ) {
		return;
	}

	// increment the visits
	aNode->visits++;

	// call on all children
	std::vector<NodeSharedPtr> children = aNode->getChildNodes();
	for(std::vector<NodeSharedPtr>::iterator it = children.begin(); it != children.end(); ++it) {
		this->pruneExtinctTipsRecursive(*it);
	}

	// get the edges leading into this node
	std::vector<EdgeSharedPtr> parentEdges = aNode->getEdgesToParents();

	// refresh children to check if we have any
	children = aNode->getChildNodes();
	if ( children.size() == 0 ) {

		// we have no descendants, time to prune

		// this node is responsible for:
		// - removing any edges leading into it from its ancestors and from the network
		// - removing itself from the node vector

		// remove edges
		for(std::vector<EdgeSharedPtr>::iterator it = parentEdges.begin(); it != parentEdges.end(); ++it) {

			// tell the parent to remove this edge (delete from vector of weak pointers to edges)
			(*it)->getParent()->removeEdge(*it);

			// remove the edge from the edge vector
			for(std::vector<EdgeSharedPtr>::iterator jt = edges.begin(); jt != edges.end(); ++jt) {
				if ( (*it) == (*jt) ) {
					edges.erase(jt); // @suppress("Invalid arguments")
					break;
				}
			}

		}

		// remove self from nodes
		for(std::vector<NodeSharedPtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
			if ( (*it) == aNode ) {
				nodes.erase(it); // @suppress("Invalid arguments")
				break;
			}
		}

	} else if ( children.size() == 1 & parentEdges.size() == 1 ) {

		// we are a knuckle

		// if this is a sampled ancestor, retain it
		if ( aNode->getType() == SampledAncestor ) {
			// don't try to delete me
			return;
		}

		// otherwise, this node is responsible for:
		// - removing the descendant edge from its descendant node
		// - attaching the ancestral edge to the descendant node
		// - removing itself from the node vector
		// - if this is a hybrid node, assign label to the descendant


		// get child edges, make sure there's only one
		std::vector<EdgeSharedPtr> edgesToChildren = aNode->getEdgesToChildren();
		assert( edgesToChildren.size() == 1 && "Knuckle node must have only one descendant edge");

		// remove the edge from the descendant
		EdgeSharedPtr edgeToRemove = edgesToChildren.at(0);
		NodeSharedPtr child = children.at(0);
		child->removeEdge(edgeToRemove);

		// set the child of the edge coming into this node to the child node
		EdgeSharedPtr edgeToKeep = parentEdges.at(0);
		edgeToKeep->setChild(child);

		// add the ancestral edge to the child
		child->addEdge(edgeToKeep);

		// remove the edge from the edge vector
		for(std::vector<EdgeSharedPtr>::iterator it = edges.begin(); it != edges.end(); ++it) {
			if ( (*it) == edgeToRemove ) {
				edges.erase(it); // @suppress("Invalid arguments")
				break;
			}
		}

		// remove self from nodes
		for(std::vector<NodeSharedPtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
			if ( (*it) == aNode ) {
				nodes.erase(it); // @suppress("Invalid arguments")
				break;
			}
		}

	} else if ( children.size() == 1 & parentEdges.size() == 2 ) {

		// we are a hybrid with one descendant

		// if this is a symmetrical hyrbridization, we need to remove the edge pointing in, then treat this as a knuckle

		// get descendant edges of child
		NodeSharedPtr child = children.at(0);
		std::vector<EdgeSharedPtr> edgesFromChild = child->getEdgesToChildren();

		// if any edge has THIS node as a child, get rid of it
		std::vector<EdgeSharedPtr>::iterator it = edgesFromChild.begin();
		for(; it != edgesFromChild.end(); ++it) {
			if ( (*it)->getChild() == aNode ) {
				// we found this node as a child
				break;
			}
		}

		// if we found an edge pointing back to this node, remove it
		if ( it != edgesFromChild.end() ) {

			// get the edge to remove
			EdgeSharedPtr edgeToRemove = *it;

			// take this edge away from the child and this node
			child->removeEdge(edgeToRemove);
			aNode->removeEdge(edgeToRemove);

			// take this edge away from the edge vector
			for(std::vector<EdgeSharedPtr>::iterator jt = edges.begin(); jt != edges.end(); ++jt) {
				if ( (*jt) == edgeToRemove ) {
					edges.erase(jt); // @suppress("Invalid arguments")
					break;
				}
			}

			// decrement visit counter and revisit this node
			// now it will be treated as a knuckle and smoother away
			aNode->visits--;
			this->pruneExtinctTipsRecursive(aNode);

		}

	}

}














} // end structure namespace
} // end network namespace
