/*
 * BaseScheduler.cpp
 *
 *  Created on: Apr 11, 2023
 *      Author: mike
 */

#include <iostream>

#include "BaseScheduler.h"
#include "EventType.h"
#include "Event.h"
#include "../../Data/Structure/IncNetworkStructure.h"

namespace Likelihood {
namespace Scheduler {

const double BaseScheduler::MAX_SEGMENT_SIZE_WITHOUT_RESCALING = 0.8;

BaseScheduler::BaseScheduler(NS::NetworkSharedPtr aPtrNetwork) : ptrNetwork(aPtrNetwork) {
	initEvents();
}

BaseScheduler::~BaseScheduler() {
	clearEvents();
}

void BaseScheduler::clearEvents() {
	for(size_t iE = 0; iE < events.size(); ++iE) {
		delete events[iE];
	}
	events.clear();
}

NS::NetworkSharedPtr BaseScheduler::getPtrNetwork() const {
	return ptrNetwork;
}

const std::vector<Event*>& BaseScheduler::getEvents() const {
	return events;
}

void BaseScheduler::initEvents() {

	// clear out old event pointers
	if(!events.empty()) {
		clearEvents();
	}

	// start at the tips
	const std::vector<NS::NodeSharedPtr> &extantNodes = ptrNetwork->getSampledTips();
	Event* startEvent = new Event(PRESENT_TIME_EVENT, extantNodes);
	events.push_back(startEvent);

	bool isValid = true;
	while( !events.back()->checkEvent(FINAL_NODE_EVENT) ) {

		// Insert the next layer of edges and return the next event (internal / root nodes)
		std::vector<NS::NodeSharedPtr> nextEventNodes = defineNextEdgesLayerAndEvent(events.back());
		assert(!nextEventNodes.empty());

		Event* nextEvent = NULL;
		if ( nextEventNodes.size() == 1 && nextEventNodes.front()->getType() == NS::Origin ) {

			// we hit the origin
			nextEvent = new Event(FINAL_NODE_EVENT, nextEventNodes.front());

		} else {

			// otherwise, we hit a node event

			// figure out which type of event occurs
			eventType_t nextEventType = this->eventTypeFromEventNodes(nextEventNodes);

			// make the event
			nextEvent = new Event(nextEventType, nextEventNodes);

			// make sure the event is valid
			isValid = isValid && nextEvent->isEventPossible();

		}

		// add the event
		events.push_back(nextEvent);

	}

	// warn us if not all the events are valid
	assert(isValid && "Not all events are valid; something went wrong");


}

eventType_t BaseScheduler::eventTypeFromEventNodes(std::vector<NS::NodeSharedPtr> eventNodes) {

	eventType_t eventType;
	size_t numNodes = eventNodes.size();

	// count number of each type of node
	std::vector<size_t> numOfEachType(9, 0);
	for(std::vector<NS::NodeSharedPtr>::iterator it = eventNodes.begin(); it != eventNodes.end(); ++it) {
		numOfEachType[(*it)->getType()] += 1;
	}

	// validate
	assert(numOfEachType[NS::HybridSpecies] <= 1 && "No more than one hybrid species allowed per event.");
	assert(numOfEachType[NS::Allopolyploid] <= 1 && "No more than one allopolyploid species allowed per event.");

	size_t isPolyploid;
	if ( numNodes == 1 ) {

		// only one node
		NS::NodeSharedPtr &thisNode = eventNodes.at(0);

		// what kind of node?
		NS::NodeType nodeType = thisNode->getType();
		if ( nodeType == NS::Speciation || nodeType == NS::Root ) {
			eventType = SPECIATION_EVENT;
		} else if ( nodeType == NS::Hybrid || nodeType == NS::HybridSpecies ) {
			eventType = HYBRID_DIAMOND;
		} else if ( nodeType == NS::Allopolyploid) {
			eventType = POLYPLOID_DIAMOND;
		} else {
			assert(false && "Could not determine type of event with one descendant.");
		}

	} else if ( numNodes == 2 ) {

		// two simultaneous nodes

		// if any of them are an allopolyploid node, this is allopolyploid 2
		isPolyploid = false;
		for(std::vector<NS::NodeSharedPtr>::iterator it = eventNodes.begin(); it != eventNodes.end(); ++it) {
			if ( (*it)->getType() == NS::Allopolyploid ) {
				isPolyploid = true;
			}
		}

		if ( isPolyploid ) {

			// this is just a polyploid event
			eventType = POLYPLOID_TRIANGLE;

		} else {

			// must be a hybridization event

			// check for reciprocal ownership
			NS::NodeSharedPtr &leftNode = eventNodes.at(0);
			NS::NodeType leftType = leftNode->getType();

			NS::NodeSharedPtr &rightNode = eventNodes.at(1);
			NS::NodeType rightType = rightNode->getType();

			bool leftOwnsRight = leftNode->hasChild(rightNode);
			bool rightOwnsLeft = rightNode->hasChild(leftNode);

			// validate
			assert( (leftOwnsRight || rightOwnsLeft) &&
					"At least one node must own the other when there are two simultaneous nodes.");
			assert( (leftOwnsRight ? (rightType == NS::Hybrid || rightType == NS::HybridSpecies) : true) &&
					"If left node ones right node, right node must be a hybrid.");
			assert( (rightOwnsLeft ? (leftType == NS::Hybrid || leftType == NS::HybridSpecies) : true) &&
					"If right node ones left node, left node must be a hybrid.");

			// decide symmetrical or asymmetrical
			if ( leftOwnsRight != rightOwnsLeft ) { /* XOR */
				eventType = DIRECTIONAL_TRIANGLE;
			} else {
				eventType = BIDIRECTIONAL_TRIANGLE;
			}

		}

	} else {

		assert( numNodes == 3 && "Should only reach here if three nodes are in event.");

		// could be an allopolyploid or hybrid speciation event
		bool isPolyploid = false;
		for(std::vector<NS::NodeSharedPtr>::iterator it = eventNodes.begin(); it != eventNodes.end(); ++it) {
			if ( (*it)->getType() == NS::Allopolyploid ) {
				isPolyploid = true;
				break;
			}
		}

		if ( isPolyploid ) {
			eventType = NEW_POLYPLOID_TRIANGLE;
		} else {
			eventType = NEW_HYBRID_TRIANGLE;
		}

	}


	return eventType;

}

std::vector<NS::NodeSharedPtr> BaseScheduler::defineNextEdgesLayerAndEvent(Event *lastEvent) {

	const std::vector<NS::NodeSharedPtr> &currentNodes = lastEvent->getNodes();

	// Find the next layer of edge
	edgesList_t nextEdges;
	if ( lastEvent->checkEvent(PRESENT_TIME_EVENT) ) {

		// Current nodes contains all the leaf nodes
		// 1) we must add all edges
		// 2) we must find the next candidate ancestral node (event)
		for (size_t iN = 0; iN < currentNodes.size(); ++iN) {
			std::vector<NS::EdgeSharedPtr> edges = currentNodes[iN]->getEdgesToParents();
			for (std::vector<NS::EdgeSharedPtr>::iterator it = edges.begin(); it != edges.end(); ++it) {
				nextEdges.push_back(*it);
			}
		}

	} else if ( lastEvent->isNodeEvent() ) {

		// Current nodes contains the ancestral node(s)

		// Starting from last edges
		nextEdges = layeredEdges.back();

		// For each ancestral node with the same age
		for(size_t iN = 0; iN < currentNodes.size(); ++iN){

			// 1) Remove old child edges (only for ancestral, not for fossil)
			std::vector<NS::EdgeSharedPtr> childrenEdges = currentNodes[iN]->getEdgesToChildren();
			for(size_t iC = 0; iC < childrenEdges.size(); ++iC) {
				nextEdges.remove(childrenEdges[iC]);
			}

			// 2) Add new parent edges, excluding horizontal edges
			std::vector<NS::EdgeSharedPtr> edges = currentNodes[iN]->getNonHorizontalEdgesToParents();
			for (std::vector<NS::EdgeSharedPtr>::iterator it = edges.begin(); it != edges.end(); ++it) {
				nextEdges.push_back(*it);
			}

		}

	} else if (lastEvent->checkEvent(FINAL_NODE_EVENT)) {

		// Current nodes contains the root node
		// Check that we only have the last two edges as the last layer
		assert(layeredEdges.back().size() == 2);
		assert(false); // We should not arrive here

		// force an empty return
		std::vector<NS::NodeSharedPtr> empty;
		return empty;

	}

	// Add the new edges to the layers
	layeredEdges.push_back(nextEdges);

	// Find the next event(s) :
	// 1) next speciation event
	std::vector<NS::NodeSharedPtr> nextEventNodes;
	double nextEventAge = std::numeric_limits<double>::max();
	for(itEdgesList_t itE = nextEdges.begin(); itE != nextEdges.end(); ++itE) {

		NS::EdgeSharedPtr edge = (*itE);

		if (edge->getParent()->getAge() < nextEventAge) { // If sooner that current event, clear and memorize
			nextEventAge = edge->getParent()->getAge();
			nextEventNodes.clear();
			nextEventNodes.push_back(edge->getParent());
		} else if (edge->getParent()->getAge() == nextEventAge && // If same age and different node, add
					  std::find(nextEventNodes.begin(), nextEventNodes.end(), edge->getParent()) == nextEventNodes.end() ) {
			nextEventNodes.push_back(edge->getParent());
		}
	}

	return nextEventNodes;

}


size_t BaseScheduler::getNumEdgesForLayer(size_t iLayer) {
	edgesList_t &theEdges = layeredEdges.at(iLayer);
	return theEdges.size();
}

bool BaseScheduler::hasBeenUpdated() const {
	return updated;
}

void BaseScheduler::clearHasBeenUpdatedFlag() {
	updated = false;
}

} /* namespace Scheduler */
} /* namespace Likelihood */
