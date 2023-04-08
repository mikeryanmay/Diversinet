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
		parent(aParent), child(aChild) { // @suppress("Symbol is not resolved")
}

Edge::~Edge() {
}

NodeSharedPtr Edge::getParent() {
	NodeSharedPtr sharedParent = parent.lock();
	assert(sharedParent != nullptr && "Parent is invalid pointer.");
	return sharedParent;
}

void Edge::setParent(NodeSharedPtr aParent) {
	parent = aParent;
}

NodeSharedPtr Edge::getChild() {
	NodeSharedPtr sharedChild = child.lock();
	assert(sharedChild != nullptr && "Parent is invalid pointer.");
	return sharedChild;
}

void Edge::setChild(NodeSharedPtr aChild) {
	child = aChild;
}

EdgeType Edge::getType() const {
	return type;
}

void Edge::setType(EdgeType aType) {
	type = aType;
}

double Edge::getLength() const {
	return parent.lock()->getAge() - child.lock()->getAge();
}

} /* namespace Structure */
} /* namespace Network */
