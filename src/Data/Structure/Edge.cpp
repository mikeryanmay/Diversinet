/*
 * Edge.cpp
 *
 *  Created on: Apr 2, 2023
 *      Author: mike
 */

#include "Edge.h"
#include "Node.h"
#include "IncFwdNetworkStructure.h"

namespace Data {
namespace Structure {

Edge::Edge(NodeSharedPtr aParent, NodeSharedPtr aChild) :
		parent(aParent), child(aChild) {

	assert(parent != NULL);
	assert(child != NULL);

}

Edge::~Edge() {
}

NodeSharedPtr Edge::getParent() {
	return parent;
}

void Edge::setParent(NodeSharedPtr aParent) {
	parent = aParent;
	assert(parent != NULL);
}

NodeSharedPtr Edge::getChild() {
	return child;
}

void Edge::setChild(NodeSharedPtr aChild) {
	child = aChild;
	assert(child != NULL);
}

EdgeType Edge::getType() const {
	return type;
}

void Edge::setType(EdgeType aType) {
	type = aType;
}


double Edge::getLength() const {
	return parent->getAge() - child->getAge();
}

} /* namespace Structure */
} /* namespace Network */
