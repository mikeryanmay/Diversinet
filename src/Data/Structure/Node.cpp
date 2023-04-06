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

bool Node::validateType() {

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
	} else if (type == Hybrid || type == HybridSpeciation) {
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


std::vector<EdgeSharedPtr> Node::getEdgesToParents() const {

	std::vector<EdgeSharedPtr> edgesToParents;

	for (size_t iE = 0; iE < edges.size(); ++iE) {
		if (edges[iE]->child.get() == this) {
			edgesToParents.push_back(edges[iE]);
		}
	}

	return edgesToParents;

}

std::vector<EdgeSharedPtr> Node::getEdgesToChildren() const {

	std::vector<EdgeSharedPtr> edgesToChildren;

	for (size_t iE = 0; iE < edges.size(); ++iE) {
		if (edges[iE]->parent.get() == this) {
			edgesToChildren.push_back(edges[iE]);
		}
	}

	return edgesToChildren;

}

std::vector<NodeSharedPtr> Node::getParentNodes() const {

	std::vector<EdgeSharedPtr> edgesToParents = this->getEdgesToParents();
	std::vector<NodeSharedPtr> parentNodes;

	for(size_t iE = 0; iE < edgesToParents.size(); ++iE) {
		parentNodes.push_back(edgesToParents[iE]->parent);
	}

	return parentNodes;

}

std::vector<NodeSharedPtr> Node::getChildNodes() const {

	std::vector<EdgeSharedPtr> edgesToChildren = this->getEdgesToChildren();
	std::vector<NodeSharedPtr> childrenNodes;

	for(size_t iE = 0; iE < edgesToChildren.size(); ++iE) {
		childrenNodes.push_back(edgesToChildren[iE]->child);
	}

	return childrenNodes;

}

void Node::addEdge(EdgeSharedPtr aEdge) {
	assert(aEdge->parent.get() == this || aEdge->child.get() == this); // Check that this is a meaningful edges
	for(size_t iE = 0; iE < edges.size(); ++iE) { // Check that each edge is unique
		assert(!(aEdge->child.get() == edges[iE]->child.get() && aEdge->parent.get() == edges[iE]->parent.get()));
	}
	edges.push_back(aEdge);
}

void Node::resetVisits() {
	visits = 0;
}

std::string Node::recursivelyConstructNewickString(EdgeSharedPtr incomingEdge) {

	std::string newick = "";

	// get the branch length
	double bl = incomingEdge->getLength();
	std::string bls = std::to_string(bl);
//	std::string bls = bl < 1e-16 ? "1e-16" : std::to_string(bl);

	// deal with multiple visits
	visits++;
	if (visits > 1) {

		// treat this like a hybrid edge (don't traverse down)
//		newick += label + "#H" + std::to_string(id) + ":" + bls;
		newick += std::to_string(id) + "#" + label + ":" + bls;
		return newick;

	}

	// terminate if we've hit the end of the road
	if ( type == Sample || type == Extinction ) {
		// add label plus branch length, then return prematurely
		newick += label + ":" + bls;
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
		assert( (type == Hybrid || type == HybridSpeciation) && "Only hybrid and hybrid-speciation nodes can have one descendant." );

		// simply call on the only child
		newick += edgesToChildren.at(0)->child->recursivelyConstructNewickString(edgesToChildren.at(0));

	} else if ( numChildren == 2 ) {

		// NOTE: deal with symmetrical hybridization

		// only call recursively if:
		// 1: not a hybridization edge
		// 2: child is a hybrid speciation node

		EdgeSharedPtr leftEdge = edgesToChildren.at(0);
		if ( !leftEdge->type == Hybridization || leftEdge->child->type == HybridSpeciation ) {
			// safe to call recursively
			newick += leftEdge->child->recursivelyConstructNewickString(leftEdge);
		} else {
			// duplicate the node string without doing anything recursive
			double new_bl = leftEdge->getLength();
			std::string new_bls = std::to_string(new_bl);
//			std::string new_bls = new_bl < 1e-16 ? "1e-16" : std::to_string(new_bl);
//			newick += leftEdge->child->label + "#LGT" + std::to_string(leftEdge->child->id) + ":" + new_bls;
			newick += std::to_string(leftEdge->child->id) + "#" + leftEdge->child->label + ":" + new_bls;

		}

		// comma
		newick += ",";

		EdgeSharedPtr rightEdge = edgesToChildren.at(1);
		if ( !rightEdge->type == Hybridization || rightEdge->child->type == HybridSpeciation ) {
			// safe to call recursively
			newick += rightEdge->child->recursivelyConstructNewickString(rightEdge);
		} else {
			// duplicate the node string without doing anything recursive
			double new_bl = rightEdge->getLength();
			std::string new_bls = std::to_string(new_bl);
//			std::string new_bls = new_bl < 1e-16 ? "1e-16" : std::to_string(new_bl);
//			newick += rightEdge->child->label + "#LGT" + std::to_string(rightEdge->child->id) + ":" + new_bls;
			newick += std::to_string(rightEdge->child->id) + "#" + rightEdge->child->label + ":" + new_bls;
		}

	}

	// add close parens
	newick += ")";

	// add label for hybridization events
	if ( type == Hybrid ) {

		// add the event indicator + id
//		newick += label + "#LGT" + std::to_string(id) + ":" + bls;
		newick += std::to_string(id) + "#" + label + ":" + bls;

	} else if ( type == HybridSpeciation ) {

		// add the event indicator + id
//		newick += label + "#H" + std::to_string(id) + ":" + bls;
		newick += std::to_string(id) + "#" + label + ":" + bls;

	} else {

		newick += ":" + bls;

	}

	return newick;

}

} /* namespace Structure */
} /* namespace Network */
