/*
 * BaseScheduler.cpp
 *
 *  Created on: Apr 11, 2023
 *      Author: mike
 */

#include <limits>
#include <iterator>
#include <iostream>
#include <vector>
#include <cmath>

#include "BaseScheduler.h"
#include "EventType.h"
#include "Event.h"
#include "../../Data/Structure/IncNetworkStructure.h"

namespace Likelihood {
namespace Scheduler {

const double BaseScheduler::MAX_SEGMENT_SIZE_WITHOUT_RESCALING = 0.02;
const size_t BaseScheduler::NUM_RECALING_EVENTS = 100;

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

void BaseScheduler::removeRescalingEvents() {
	this->removeEventsByType(RESCALING_EVENT);
	updated = true;
}

void BaseScheduler::removeEventsByType(eventType_t aEventType) {

	std::vector<Event*>::iterator it = events.begin();

	while(it != events.end()) { // Looping through event and removing async. rate shifts
		if((*it)->checkEvent(aEventType)) {

			// Finding the position of the event
			size_t pos = std::distance(events.begin(), it); // @suppress("Invalid arguments")
			// Erase the associated layer of edges
			layeredEdges.erase(layeredEdges.begin() + (pos - 1)); // @suppress("Invalid arguments")
			// Erase the event
			delete (*it);
			it = events.erase(it); // @suppress("Invalid arguments")

		} else {
			it++;
		}
	}

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
			// isValid = isValid && nextEvent->isEventPossible();

		}

		// std::cout << "found event of type " << nextEvent->getEvent() << " at time " << nextEvent->getTime() << std::endl;

		// add the event
		events.push_back(nextEvent);

	}

	// warn us if not all the events are valid
	// assert(isValid && "Not all events are valid; something went wrong");

}

void BaseScheduler::defineAndSetRescalingEvents() {

	removeRescalingEvents();

	// get the oldest node in the network
	double startTime = ptrNetwork->getOldestNode()->getAge();

	// get all the node ages
	const std::vector<NS::NodeSharedPtr>& allNodes = ptrNetwork->getNodes();
	std::vector<double> nodeAges(allNodes.size());
	for(int iN = 0; iN < allNodes.size(); ++iN) {
		nodeAges[iN] = allNodes[iN]->getAge();
	}

	// compute delta T between events
	double rescalingInterval = startTime / (double)(NUM_RECALING_EVENTS + 1);

	// create the rescaling events
	double currentTime = 0.0;
	std::vector<Data::Structure::NodeSharedPtr> rescalingEventNodes;
	for(int iR = 0; iR < NUM_RECALING_EVENTS; ++iR) {

		// get the time
		currentTime += rescalingInterval;

		// check if this event is simultaneous with a node event
		if (std::find(nodeAges.begin(), nodeAges.end(), currentTime) == nodeAges.end()) {
			
			// if not, create the new rescaling event
			int index = getFirstEventAfterTimeT(currentTime);
			Event* newEvent = new Event(RESCALING_EVENT, currentTime, rescalingEventNodes);
			std::vector<Event*>::iterator itE = events.begin();
			std::advance(itE, index);
			events.insert(itE, newEvent); // @suppress("Invalid arguments")

			// update the edges layer
			std::vector< edgesList_t >::iterator itL = layeredEdges.begin();
			std::advance(itL, index-1);
			edgesList_t layerCopy  = *itL;
			layeredEdges.insert(itL, layerCopy); // @suppress("Invalid arguments")

		}
		
	}

	// std::vector< std::pair<double, size_t> > rescalingTime; // Trick to get the argsort
	// std::vector<Data::Structure::NodeSharedPtr> nodesId;
	// std::vector<bool> associatedToOtherEvent;

	// // For each edges
	// const std::vector<Data::Structure::EdgeSharedPtr>& edges = ptrNetwork->getEdges();
	// for(size_t iE=0; iE<edges.size(); ++iE) {
	// 	// If the edges is longer than a given length, we request for rescaling events
	// 	if(edges[iE]->getLength() > MAX_SEGMENT_SIZE_WITHOUT_RESCALING) {
	// 		double edgeLength = edges[iE]->getLength();
	// 		size_t nSubSegment = std::floor(edgeLength/MAX_SEGMENT_SIZE_WITHOUT_RESCALING);
	// 		double subsegmentSize = edgeLength/nSubSegment;

	// 		//std::cout << "Treating edge : " << edges[iE]->toString();
	// 		//std::cout << "With child node : " << edges[iE]->getChild()->toString();

	// 		// Request a rescaling at time child_node + i*subsegment_length
	// 		for(size_t iS=1; iS<nSubSegment; ++iS) {
	// 			double requestedTime = edges[iE]->getChild()->getAge()+subsegmentSize*iS;
	// 			//std::cout << "Subsegment boundary " << iS << " with requested time = " << requestedTime << std::endl;

	// 			int firstEventAfterReqTime = getFirstEventAfterTimeT(requestedTime);
	// 			assert(firstEventAfterReqTime >= 0);

	// 			int firstEventPriorReqTime = firstEventAfterReqTime - 1;
	// 			assert(firstEventAfterReqTime >= 0);

	// 			// if it is within a 30% subsegment length tolerance of the requested time, we update the req time
	// 			if(std::fabs(events[firstEventAfterReqTime]->getTime() - requestedTime) < (0.3*MAX_SEGMENT_SIZE_WITHOUT_RESCALING)/2.) {
	// 				requestedTime = events[firstEventAfterReqTime]->getTime();
	// 				associatedToOtherEvent.push_back(true);
	// 				//std::cout << "Updated to next event  :" << events[firstEventAfterReqTime]->toString() << std::endl;
	// 			} else if(std::fabs(events[firstEventPriorReqTime]->getTime() - requestedTime) < (0.3*MAX_SEGMENT_SIZE_WITHOUT_RESCALING)/2.) {
	// 				requestedTime = events[firstEventPriorReqTime]->getTime();
	// 				//std::cout << "Updated to prior event  :" << events[firstEventPriorReqTime]->toString() << std::endl;
	// 				associatedToOtherEvent.push_back(true);
	// 			} else {
	// 				associatedToOtherEvent.push_back(false);
	// 			}

	// 			nodesId.push_back(edges[iE]->getChild());
	// 			rescalingTime.push_back(std::make_pair(requestedTime, rescalingTime.size())); // @suppress("Invalid arguments")
	// 			//std::cout << "Final time  :" << requestedTime << std::endl;
	// 		}
	// 		//std::cout << "--------------------------------------------" << std::endl;
	// 	}
	// }

	// if(rescalingTime.empty()) return; // no rescaling times, we are out

	// updated = true;

	// // Otherwise, regroup them and add events
	// // Order and regroup events
	// std::sort(rescalingTime.begin(), rescalingTime.end()); // Getting the sorted time and argsort

	// // First element
	// double firstTime = -1;
	// std::vector<Data::Structure::NodeSharedPtr> rescalingEventNodes;
	// for(size_t iR=0; iR<rescalingTime.size(); ++iR) {

	// 	double time = rescalingTime[iR].first;
	// 	if(firstTime < 0.) firstTime = time;

	// 	size_t pos = rescalingTime[iR].second;

	// 	rescalingEventNodes.push_back(nodesId[pos]);

	// 	// There is 3 reasons to create an event at this point:
	// 	// 1) We are the last rescaling event
	// 	// 2) There is already a synchronous event at this time and the next rescaling event differ in time
	// 	// 3) The difference between rescaling events unrelated to an existing synchronous event exceed 30% of the MAX_SEGMENT_SIZE
	// 	if(iR == rescalingTime.size()-1 ||
	// 	   (associatedToOtherEvent[pos] && rescalingTime[iR+1].first != time) ||
	// 			(rescalingTime[iR+1].first - firstTime) > 0.3*MAX_SEGMENT_SIZE_WITHOUT_RESCALING) {

	// 		if(!associatedToOtherEvent[pos]) { // if we are not associated, we take the mean time
	// 			time = 0.;
	// 			for(size_t iS=0; iS<rescalingEventNodes.size(); ++iS) {
	// 				time += rescalingTime[iR-iS].first/rescalingEventNodes.size();
	// 			}
	// 			//std::cout << "We are a bunch of free rescaling, thus averaging time to t = " << time << std::endl;
	// 		} else {
	// 			//std::cout << "We are associated with an event, thus keeping time t = " << time << std::endl;
	// 		}

	// 		// We create and insert an event
	// 		int index = getFirstEventAfterTimeT(time);
	// 		Event* newEvent = new Event(RESCALING_EVENT, time, rescalingEventNodes);
	// 		//std::cout << "Creating event : " << newEvent->toString() << std::endl;
	// 		std::vector<Event*>::iterator itE = events.begin();
	// 		std::advance(itE, index);
	// 		events.insert(itE, newEvent); // @suppress("Invalid arguments")

	// 		// update the edges layer
	// 		std::vector< edgesList_t >::iterator itL = layeredEdges.begin();
	// 		std::advance(itL, index-1);
	// 		edgesList_t layerCopy  = *itL;
	// 		layeredEdges.insert(itL, layerCopy); // @suppress("Invalid arguments")

	// 		// We clear the rescalingEventNode buffer
	// 		rescalingEventNodes.clear();
	// 		firstTime = -1.;
	// 	}
	// }
	// //std::cout << "--------------------------------------------" << std::endl;

}

int BaseScheduler::getFirstEventAfterTimeT(double aTime) {
	for(size_t iE=0; iE < events.size(); ++iE) {
		//std::cout << "[GET] Event time = " << aTime << " vs event time =  " << events[iE]->getTime() << " -- " << std::scientific << fabs(aTime - events[iE]->getTime()) << (aTime == events[iE]->getTime() ? " -- SAME!" : "-- not same??") << std::endl;
		if(aTime <= events[iE]->getTime()) {
			return iE;
		} else if (std::fabs(aTime - events[iE]->getTime()) <= 2.*std::numeric_limits<double>::epsilon()) {
			return iE;
		}
	}
	return -1;
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
	// assert(numOfEachType[NS::HybridSpecies] <= 1 && "No more than one hybrid species allowed per event.");
	// assert(numOfEachType[NS::Allopolyploid] <= 1 && "No more than one allopolyploid species allowed per event.");

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

			// // validate
			// assert( (leftOwnsRight || rightOwnsLeft) &&
			// 		"At least one node must own the other when there are two simultaneous nodes.");
			// assert( (leftOwnsRight ? (rightType == NS::Hybrid || rightType == NS::HybridSpecies) : true) &&
			// 		"If left node ones right node, right node must be a hybrid.");
			// assert( (rightOwnsLeft ? (leftType == NS::Hybrid || leftType == NS::HybridSpecies) : true) &&
			// 		"If right node ones left node, left node must be a hybrid.");
			// MRM 2025: leave this to isEventsValid step so we can jitter if necessary

			// decide symmetrical or asymmetrical
			if ( leftOwnsRight != rightOwnsLeft ) { /* XOR */
				eventType = DIRECTIONAL_TRIANGLE;
			} else {
				eventType = BIDIRECTIONAL_TRIANGLE;
			}

		}

	} else {

		// assert( numNodes == 3 && "Should only reach here if three nodes are in event.");

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

		if (std::fabs(edge->getParent()->getAge() - nextEventAge) < 1e-6 && // If same age and different node, add
			std::find(nextEventNodes.begin(), nextEventNodes.end(), edge->getParent()) == nextEventNodes.end() ) {
			nextEventNodes.push_back(edge->getParent());
		} else if (edge->getParent()->getAge() < nextEventAge) {
			nextEventAge = edge->getParent()->getAge();
			nextEventNodes.clear();
			nextEventNodes.push_back(edge->getParent());
		}
		
		// if (edge->getParent()->getAge() < nextEventAge) { // If sooner that current event, clear and memorize
		// 	nextEventAge = edge->getParent()->getAge();
		// 	nextEventNodes.clear();
		// 	nextEventNodes.push_back(edge->getParent());
		// } else if (edge->getParent()->getAge() == nextEventAge && // If same age and different node, add
		// 			  std::find(nextEventNodes.begin(), nextEventNodes.end(), edge->getParent()) == nextEventNodes.end() ) {
		// 	nextEventNodes.push_back(edge->getParent());
		// }

	}

	// make sure all the nodes have the same age
	for(std::vector<NS::NodeSharedPtr>::iterator it = nextEventNodes.begin(); it != nextEventNodes.end(); ++it) {
		(*it)->setAge(nextEventAge);
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
