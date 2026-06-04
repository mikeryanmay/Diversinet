/*
 * Tree.cpp
 *
 *  Created on: Apr 2, 2023
 *      Author: mike
 */

#include <iostream>
#include <algorithm>
#include <set>

#include <boost/make_shared.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

#include "Network.h"
#include "Node.h"
#include "Edge.h"

#include "../Reader/IncPhyloReader.h"

namespace Data {
namespace Structure {

Network::Network(std::vector<NodeSharedPtr> someNodes, std::vector<EdgeSharedPtr> someEdges) :
	nodes(someNodes), edges(someEdges) {

	// make sure we have an oldest node
	// this->updateOldestNode();

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

	// set root and origin
	originNode = origin;
	rootNode = root;

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
	for(size_t iL = 0; iL + 1 < labels.size(); ++iL) {
		if ( labels[iL] == labels[iL+1] ) {
			hybidLabels.push_back(labels[iL]);
			++iL;
		}
	}

	// merge nodes by label
	for(size_t iH = 0; iH < hybidLabels.size(); ++iH) {
		this->mergeHybridNodesByLabels( hybidLabels[iH] );
	}

	// update the nodes
	// this->updateOldestNode();
	this->ensureSimultaneousEvents();
	this->updateNodes();

}

void Network::mergeHybridNodesByLabels(std::string aLabel) {

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
	// NodeSharedPtr originNode = this->getOldestNode();

	// call recursive function on oldest node
	assert(originNode && "must have an origin node");
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

size_t Network::getNumSampledTips() const {
	return sampledTips.size();
}

size_t Network::getNumExtinctTips() const {
	return extinctTips.size();
}

size_t Network::getNumHybridNodes() const {
	return hybridNodes.size();
}

const std::vector<NodeSharedPtr>& Network::getExtinctTips() const {
	return extinctTips;
}

void Network::updateOldestNode() {

	// loop over nodes, finding oldest one
	oldestNode = originNode;
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
	originNode.reset();
	rootNode.reset();
	oldestNode.reset();

	// don't do anything if there are no nodes
	if (nodes.size() == 0) {
		isExtinct = true;
		return;
	} else {
		isExtinct = false;
	}

	// loop over nodes
	size_t numOrigins = 0;
	size_t numRoots = 0;
	for(std::vector<NodeSharedPtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ( (*it)->getType() == Sample ) {
			sampledTips.push_back(*it);
		} else if ( (*it)->getType() == Extinction ) {
			extinctTips.push_back(*it);
		} else if ( (*it)->getType() == Hybrid || (*it)->getType() == HybridSpecies || (*it)->getType() == Allopolyploid ) {
			hybridNodes.push_back(*it);
		} else if ( (*it)->getType() == Origin ) {
			originNode = *it;
			numOrigins++;
		} else if ( (*it)->getType() == Root ) {
			rootNode = *it;
			numRoots++;
		}
	}

	// make sure we didn't find too many root and origin nodes
	assert(numOrigins == 1 && "did not find an origin node");
	assert(numRoots == 1 && "did not find a root node");

	// flag whether we are extinct
	isExtinct = sampledTips.size() == 0;

	// update the oldest node
	this->updateOldestNode();
	
	// check that originNode and rootNode exist
	assert(originNode && "no origin node found");
	assert(rootNode   && "no root node found");
	
	// check that originNode is oldestNode
	assert(originNode == oldestNode && "origin node is not the oldest node");

}

std::string Network::getNewickString() {

	if (isExtinct) {
		return "";
	}

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
	// NodeSharedPtr originNode = this->getOldestNode();

	// call recursive function on oldest node
	this->pruneExtinctTipsRecursive(originNode);

	// update the oldest node
	this->updateOldestNode();

	// reset root and origin
	rootNode->setType(Speciation);
	if (oldestNode->getParentNodes().size() == 0 && oldestNode->getChildNodes().size() == 1) {
		// oldest node is the origin node
		oldestNode->setType(Origin);
		oldestNode->getChildNodes()[0]->setType(Root);
		originNode = oldestNode;
		rootNode   = oldestNode->getChildNodes()[0];
	} else if (oldestNode->getParentNodes().size() == 0 && oldestNode->getChildNodes().size() == 2) {
		// this is the root node, but it has no origin node
		oldestNode->setType(Root);
		rootNode = oldestNode;
		originNode = boost::make_shared<Node>( Node(0, rootNode->getAge(), Origin) );
		EdgeSharedPtr stem = boost::make_shared<Edge>( Edge(originNode, rootNode) );
		nodes.push_back(originNode);
		edges.push_back(stem);
		originNode->addEdge(stem);
		rootNode->addEdge(stem);
	} else if (oldestNode->getParentNodes().size() == 1 && oldestNode->getChildNodes().size() == 2) {
		// this is the root node, but it has an origin node parent
		oldestNode->setType(Root);
		oldestNode->getParentNodes()[0]->setType(Origin);
		originNode = oldestNode->getParentNodes()[0];
		rootNode   = oldestNode;
	} else {
		assert("How did we get here?");
	}
	
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

	} else if ( children.size() == 1 && parentEdges.size() == 1 ) {

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

	} else if ( children.size() == 1 && parentEdges.size() == 2 ) {

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

void Network::jitterNetwork(double factor) {
	
	// prepare the RNG
	boost::random::random_device dev;
	rng.seed(dev);

	// first, make an empty set of ages
	std::set<double> ages;

	// get the oldest node
	NodeSharedPtr originNode = this->getOldestNode();

	// call recursive function on oldest node
	this->jitterNetworkRecursive(originNode, ages, factor);
	
}

void Network::jitterNetworkRecursive(NodeSharedPtr node, std::set<double>& ages, double factor) {

	// if this node is a tip, terminate
	if (node->getType() == Sample) {
		return;
	}

	// call recursively on children
	std::vector<NodeSharedPtr> children = node->getChildNodes();
	for(std::vector<NodeSharedPtr>::iterator it = children.begin(); it != children.end(); ++it) {
		this->jitterNetworkRecursive(*it, ages, factor);
	}

	// otherwise, get the age
	const double& nodeAge = node->getAge();

	// try to insert the node age
	bool wasInserted = ages.insert(nodeAge).second;

	// only jitter if this is a speciation node
	if (node->getType() != Speciation) {
		return;
	}
	
	// if we failed to insert...
	if (!wasInserted) {

		// have to jitter the age
		// std::cout << "found duplicate node age at " << nodeAge << std::endl;
		// std::cout << "node is of type: " << node->getType() << std::endl;

		// 1: get max age
		std::vector<NodeSharedPtr> parents = node->getParentNodes();
		double maxAge = nodeAge + factor;
		for(std::vector<NodeSharedPtr>::iterator jt = parents.begin(); jt != parents.end(); ++jt) {
			if ( (*jt)->getAge() < maxAge ) {
				maxAge = (*jt)->getAge();
			}
		}

		// 2. get min age
		double minAge = nodeAge - factor;
		for(std::vector<NodeSharedPtr>::iterator jt = children.begin(); jt != children.end(); ++jt) {
			if ( (*jt)->getAge() > minAge ) {
				minAge = (*jt)->getAge();
			}
		}

		// 3. draw a new age uniformly between min and max
		boost::uniform_real<> ageDist(minAge, maxAge);
		double newAge = ageDist(rng);
		// double newAge = nodeAge - 0.0001;

		// 4. set the new age
		node->setAge(newAge);

		// std::cout << "new age is " << node->getAge() << std::endl;
		
	} // end duplicate age case

	// nothing left to do

}


bool Network::hasStem() const {

	// check whether the network has a stem
	// practically, the network has a stem if the 
	// branch length between the origin and the root is 
	// greater than zero

	if (isExtinct) {
		return false;
	}

	return fabs(originNode->getAge() - rootNode->getAge()) > 1e-10;

}

bool Network::isExtant() const {
	return !isExtinct;
}



} // end structure namespace
} // end network namespace
