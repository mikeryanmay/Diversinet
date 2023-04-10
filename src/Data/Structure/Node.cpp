/*
 * Node.cpp
 *
 *  Created on: Apr 2, 2023
 *      Author: mike
 */

#include <iostream>
#include "Node.h"
#include "Edge.h"
#include "IncFwdNetworkStructure.h"

namespace Data {
namespace Structure {

Node::Node(size_t aId, double aAge, NodeType aType) : id(aId), age(aAge), type(aType) {
}

Node::~Node() {
}

NodeType Node::getType() const {
	return type;
}

void Node::setType(NodeType aType) {
	type = aType;
}

bool Node::validateType() const {

	// get parents and children
	std::vector<NodeSharedPtr> parents  = this->getParentNodes();
	std::vector<NodeSharedPtr> children = this->getChildNodes();

	// count number of parents and children
	size_t num_parents  = parents.size();
	size_t num_children = children.size();

	// check whether we have the right number of ancestors/descendants
	bool isValid = false;
	if (type == Origin) {
		isValid = num_parents == 0 & num_children == 1;
	} else if (type == Root) {
		isValid = num_parents == 0 & num_children == 2;
	} else if (type == Sample) {
		isValid = num_parents == 1 & num_children == 0;
	} else if (type == Speciation) {
		isValid = num_parents == 1 & num_children == 2;
	} else if (type == Extinction) {
		isValid = num_parents == 1 & num_children == 0;
	} else if (type == Donor) {
		isValid = num_parents == 1 & num_children == 2;
	} else if (type == Hybrid || type == HybridSpecies || type == Allopolyploid) {
		isValid = num_parents == 2 & num_children > 0;
	}

	return isValid;

}

size_t Node::getId() const {
	return id;
}

std::string Node::getLabel() const {
	return label;
}

void Node::setLabel(std::string aLabel) {
	label = aLabel;
}

void Node::setAge(double aAge) {
	age = aAge;
}

double Node::getAge() const {
	return age;
}

std::vector<EdgeSharedPtr> Node::getEdges() const {

	std::vector<EdgeSharedPtr> newEdges;
	for(size_t iE = 0; iE < edges.size(); ++iE) {

		// get a shared pointer to the edge
		EdgeSharedPtr newEdge = edges[iE].lock();

		// check that it's not null
		assert(newEdge != nullptr && "Found an invalidated edge pointer. Something went wrong.");

		// add to vector
		newEdges.push_back(newEdge);

	}
	return newEdges;

}

std::vector<EdgeSharedPtr> Node::getEdgesToParents() const {

	std::vector<EdgeSharedPtr> edgesToParents;
	std::vector<EdgeSharedPtr> edgeSharedPtr = this->getEdges();

	for (size_t iE = 0; iE < edgeSharedPtr.size(); ++iE) {
		if (edgeSharedPtr[iE]->getChild().get() == this) {
			edgesToParents.push_back(edgeSharedPtr[iE]);
		}
	}

	return edgesToParents;

}

std::vector<EdgeSharedPtr> Node::getEdgesToChildren() const {

	std::vector<EdgeSharedPtr> edgesToChildren;
	std::vector<EdgeSharedPtr> edgeSharedPtr = this->getEdges();

	for (size_t iE = 0; iE < edges.size(); ++iE) {
		if (edgeSharedPtr[iE]->getParent().get() == this) {
			edgesToChildren.push_back(edgeSharedPtr[iE]);
		}
	}

	return edgesToChildren;

}

std::vector<NodeSharedPtr> Node::getParentNodes() const {

	std::vector<EdgeSharedPtr> edgesToParents = this->getEdgesToParents();
	std::vector<NodeSharedPtr> parentNodes;

	for(size_t iE = 0; iE < edgesToParents.size(); ++iE) {
		parentNodes.push_back(edgesToParents[iE]->getParent());
	}

	return parentNodes;

}

std::vector<NodeSharedPtr> Node::getChildNodes() const {

	std::vector<EdgeSharedPtr> edgesToChildren = this->getEdgesToChildren();
	std::vector<NodeSharedPtr> childrenNodes;

	for(size_t iE = 0; iE < edgesToChildren.size(); ++iE) {
		childrenNodes.push_back(edgesToChildren[iE]->getChild());
	}

	return childrenNodes;

}

void Node::addEdge(EdgeSharedPtr aEdge) {
	assert(aEdge->getParent().get() == this || aEdge->getChild().get() == this); // Check that this is a meaningful edges
	std::vector<EdgeSharedPtr> edgeSharedPtr = this->getEdges();
//	for(size_t iE = 0; iE < edgeSharedPtr.size(); ++iE) { // Check that each edge is unique
//		assert(!(aEdge->getChild().get() == edgeSharedPtr[iE]->getChild().get() && aEdge->getParent().get() == edgeSharedPtr[iE]->getParent().get()));
//	}
	EdgeWeakPtr newEdge = aEdge; // @suppress("Invalid arguments")
	edges.push_back(newEdge);
}

void Node::removeEdge(EdgeSharedPtr aEdge) {

	// loop over edges
	for(std::vector<EdgeWeakPtr>::iterator it = edges.begin(); it != edges.end(); ++it) {

		// get a shared ptr to the edge
		EdgeSharedPtr thisEdge = (*it).lock();

		// ensure it's valid
		assert(thisEdge != nullptr && "Found invalid edge pointer when trying to remove and edge.");

		// if they are the same edge, erase it and end
		if ( thisEdge == aEdge ) {
			edges.erase(it); // @suppress("Invalid arguments")
			break;
		}

	}

}

std::string Node::constructLabel() const {

	std::string str;

	if ( type == Sample || type == Extinction  ) {
		str = label;
	} else {
//		str = "#" + label;
		str = label;
	}

	return str;

}

void Node::resetVisits() {
	visits = 0;
}

std::string Node::recursivelyConstructNewickString(EdgeSharedPtr incomingEdge) {

	std::string newick = "";

	// get the branch length
	double bl = incomingEdge->getLength();
	std::string bls = std::to_string(bl);

	// deal with multiple visits
	visits++;
	if (visits > 1) {

		// treat this like a hybrid edge (don't traverse down)
		newick += this->constructLabel() + ":" + bls;
		return newick;

	}

	// terminate if we've hit the end of the road
	if ( type == Sample || type == Extinction ) {
		// add label plus branch length, then return prematurely
		newick += this->constructLabel() + ":" + bls;
		return newick;
	} else {
		// open a parenthesis
		newick += "(";
	}

	// call recursively on children
	std::vector<EdgeSharedPtr> edgesToChildren = this->getEdgesToChildren();
	size_t numChildren = edgesToChildren.size();

	// there should only ever be one or two children
	assert( (numChildren == 1 || numChildren == 2) && "Nodes should have one or two descendants");

	// if there is only one child...
	if ( numChildren == 1 ) {

		// this should be a hybrid node with one daughter
		assert( (type == Hybrid || type == HybridSpecies || type == Allopolyploid ) && "Only hybrid, hybrid-speciation, and allopolyploid nodes can have one descendant." );

		// simply call on the only child
		newick += edgesToChildren.at(0)->getChild()->recursivelyConstructNewickString(edgesToChildren.at(0));

	} else if ( numChildren == 2 ) {

		// only call recursively if:
		// 1: not a hybridization edge
		// 2: child is a hybrid speciation node or polyploid node

		EdgeSharedPtr leftEdge = edgesToChildren.at(0);
		if ( !leftEdge->type == Hybridization || leftEdge->getChild()->type == HybridSpecies || leftEdge->getChild()->type == Allopolyploid ) {
			// safe to call recursively
			newick += leftEdge->getChild()->recursivelyConstructNewickString(leftEdge);
		} else {
			// duplicate the node string without doing anything recursive
			double new_bl = leftEdge->getLength();
			std::string new_bls = std::to_string(new_bl);
			newick += leftEdge->getChild()->constructLabel() + ":" + new_bls;
		}

		// comma
		newick += ",";

		EdgeSharedPtr rightEdge = edgesToChildren.at(1);
		if ( !rightEdge->type == Hybridization || rightEdge->getChild()->type == HybridSpecies || rightEdge->getChild()->type == Allopolyploid ) {
			// safe to call recursively
			newick += rightEdge->getChild()->recursivelyConstructNewickString(rightEdge);
		} else {
			// duplicate the node string without doing anything recursive
			double new_bl = rightEdge->getLength();
			std::string new_bls = std::to_string(new_bl);
			newick += rightEdge->getChild()->constructLabel() + ":" + new_bls;
		}

	}

	// add close parens
	newick += ")";

	// add label for hybridization events
	newick += this->constructLabel() + ":" + bls;

	return newick;

}

} /* namespace Structure */
} /* namespace Network */
