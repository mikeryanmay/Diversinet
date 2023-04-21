/*
 * Event.cpp
 *
 *  Created on: Apr 11, 2023
 *      Author: mike
 */

#include <sstream>
#include <iostream>

#include "Event.h"
#include "../../Data/Structure/Node.h"
#include "../../Data/Structure/Edge.h"
#include "../../Data/Structure/IncFwdNetworkStructure.h"

namespace Likelihood {
namespace Scheduler {

Event::Event(eventType_t anEventType, double atTime) :
		eventType(anEventType), time(atTime) {

}

Event::Event(eventType_t anEventType, NS::NodeSharedPtr aNode) :
		eventType(anEventType), time(aNode->getAge()), eventNodes(1, aNode) {

	// make sure we have node
	assert(aNode);

}

Event::Event(eventType_t anEventType, const std::vector<NS::NodeSharedPtr> &aNodes) :
		eventType(anEventType), eventNodes(aNodes) {

	// make sure we have nodes
	assert(!eventNodes.empty() && eventNodes.front());

	// set time
	time = eventNodes.front()->getAge();

}

Event::Event(eventType_t anEventType, double atTime, const std::vector<NS::NodeSharedPtr> &aNodes) :
		eventType(anEventType), time(atTime), eventNodes(aNodes) {

	// make sure we have nodes
	assert(!eventNodes.empty());

	// this is only for rescaling events
	assert(anEventType == RESCALING_EVENT);
	for(size_t iN = 0; iN < eventNodes.size(); ++iN) {
		assert(eventNodes[iN]);
		assert(!(eventNodes[iN]->getType() == Data::Structure::Origin));
		assert(eventNodes[iN]->getAge() < atTime);
	}

}

Event::~Event() {
}

bool Event::checkEvent(eventType_t aEventType) const {
	return eventType == aEventType;
}

bool Event::isNodeEvent() const {
	bool isNodeEvent = eventType == SPECIATION_EVENT ||
	                   eventType == DIRECTIONAL_TRIANGLE ||
	                   eventType == BIDIRECTIONAL_TRIANGLE ||
	                   eventType == NEW_HYBRID_TRIANGLE ||
	                   eventType == HYBRID_DIAMOND ||
	                   eventType == POLYPLOID_TRIANGLE ||
	                   eventType == NEW_POLYPLOID_TRIANGLE ||
	                   eventType == POLYPLOID_DIAMOND;
	return isNodeEvent;
}

eventType_t Event::getEvent() const {
	return eventType;
}

void Event::setTime(double aTime) {
	time = aTime;
}

double Event::getTime() const {
	return time;
}

const std::vector<NS::NodeSharedPtr>& Event::getNodes() const {
	return eventNodes;
}

bool Event::isEventPossible() const {

	// check whether the event is possible based on the types of nodes it contains
	bool isValid = true;
	bool oneOwnsTwo, twoOwnsOne;
	bool oneIsAllo, twoIsAllo;
	size_t numHybridNodes = 0, numPolyNodes = 0;
	std::vector<size_t> nonHybrids, nonPolys;
	size_t hybrid = 0, poly = 0;
	switch (eventType) {
		case PRESENT_TIME_EVENT:
			isValid = !eventNodes.empty(); // must have at least one node
			for(size_t iN = 0; iN < eventNodes.size(); ++iN) { // each node must be a sample
				isValid = isValid && eventNodes[iN]->getType() == NS::Sample;
			}
			break;
		case SPECIATION_EVENT:
			isValid = eventNodes.size() == 1 && (eventNodes[0]->getType() == NS::Speciation || eventNodes[0]->getType() == NS::Root);
			break;
		case DIRECTIONAL_TRIANGLE:
			isValid = eventNodes.size() == 2; // must be two nodes
			isValid = isValid && eventNodes[0]->getAge() == eventNodes[1]->getAge(); // nodes must be same age
			oneOwnsTwo = eventNodes[0]->hasChild(eventNodes[1]);
			twoOwnsOne = eventNodes[1]->hasChild(eventNodes[0]);
			isValid = oneOwnsTwo != twoOwnsOne; // XOR, exactly one can own the other
			break;
		case BIDIRECTIONAL_TRIANGLE:
			isValid = eventNodes.size() == 2; // must be two nodes
			isValid = isValid && eventNodes[0]->getAge() == eventNodes[1]->getAge(); // nodes must be same age
			oneOwnsTwo = eventNodes[0]->hasChild(eventNodes[1]);
			twoOwnsOne = eventNodes[1]->hasChild(eventNodes[0]);
			isValid = oneOwnsTwo && twoOwnsOne; // nodes must own each other
			break;
		case NEW_HYBRID_TRIANGLE:
			isValid = eventNodes.size() == 3; // must be three nodes
			for(size_t iN = 0; iN < eventNodes.size(); ++iN) {
				if ( eventNodes[iN]->getType() == NS::HybridSpecies ) {
					numHybridNodes++;
					hybrid = iN;
				} else {
					nonHybrids.push_back(iN);
				}
			}
			isValid = isValid && numHybridNodes == 1; // can only have one hybrid node
			for(size_t iN = 0; iN < nonHybrids.size(); ++iN) {
				isValid = isValid && eventNodes[nonHybrids[iN]]->hasChild(eventNodes[hybrid]);
			}
			break;
		case HYBRID_DIAMOND:
			isValid = eventNodes.size() == 1;
			isValid = isValid && (eventNodes[0]->getType() == NS::Hybrid || eventNodes[0]->getType() == NS::HybridSpecies);
			break;
		case POLYPLOID_DIAMOND:
			isValid = eventNodes.size() == 1 && eventNodes[0]->getType() == NS::Allopolyploid;
			break;
		case POLYPLOID_TRIANGLE:
			isValid = eventNodes.size() == 2; // must be two nodes
			isValid = isValid && eventNodes[0]->getAge() == eventNodes[1]->getAge(); // nodes must be same age
			oneIsAllo = eventNodes[0]->getType() == NS::Allopolyploid;
			twoIsAllo = eventNodes[1]->getType() == NS::Allopolyploid;
			isValid = isValid && oneIsAllo != twoIsAllo; // exactly one must be an allopolyploid
			isValid = isValid && eventNodes[oneIsAllo]->hasChild(eventNodes[twoIsAllo]); // the allopolyploid must be owned by the other node
			break;
		case NEW_POLYPLOID_TRIANGLE:
			isValid = eventNodes.size() == 3; // must be three nodes
			for(size_t iN = 0; iN < eventNodes.size(); ++iN) {
				if ( eventNodes[iN]->getType() == NS::Allopolyploid ) {
					numPolyNodes++;
					poly = iN;
				} else {
					nonPolys.push_back(iN);
				}
			}
			isValid = isValid && numPolyNodes == 1; // can only have one polyploid node
			for(size_t iN = 0; iN < nonPolys.size(); ++iN) {
				isValid = isValid && eventNodes[nonPolys[iN]]->hasChild(eventNodes[poly]);
			}
			break;
		case FINAL_NODE_EVENT:
			isValid = eventNodes.size() == 1 &&
				eventNodes.front()->getType() == NS::Origin;
			break;
		case RESCALING_EVENT:
			break;
		default:
			assert(false && "Unregistered event type");
			break;
	}

	if ( isValid == false ) {
		std::cout << "This " << eventType << " is invalid." << std::endl;
	}

	return isValid;

}

std::string Event::toString() const {

	std::stringstream ss;

	switch (eventType) {
		case PRESENT_TIME_EVENT:
			ss << "Present time (all extant taxa).";
			ss << std::endl << "Nodes : [ ";
			for(size_t iN=0; iN<eventNodes.size(); ++iN) {
				ss << eventNodes[iN]->getId() << " ,";
			}
			ss << "]";
			break;
		case SPECIATION_EVENT:
			assert(!eventNodes.empty());
			ss << "Internal node : [ ";
			for(size_t iN=0; iN<eventNodes.size(); ++iN) {
				ss << eventNodes[iN]->getId() << " ,";
			}
			ss << "] - age = " << time;
			break;
		case DIRECTIONAL_TRIANGLE:
			ss << "Asymmetrical hybridization event : " << "  time = " << time;
			ss << std::endl << "Nodes : [ ";
			for(size_t iN=0; iN<eventNodes.size(); ++iN) {
				ss << eventNodes[iN]->getId() << " ,";
			}
			ss << "]";
			break;
		case BIDIRECTIONAL_TRIANGLE:
			ss << "Symmetrical hybridization event : " << "  time = " << time;
			ss << std::endl << "Nodes : [ ";
			for(size_t iN=0; iN<eventNodes.size(); ++iN) {
				ss << eventNodes[iN]->getId() << " ,";
			}
			ss << "]";
			break;
		case NEW_HYBRID_TRIANGLE:
			ss << "Hybrid speciation event : " << "  time = " << time;
			ss << std::endl << "Nodes : [ ";
			for(size_t iN=0; iN<eventNodes.size(); ++iN) {
				ss << eventNodes[iN]->getId() << " ,";
			}
			ss << "]";
			break;
		case HYBRID_DIAMOND:
			ss << "Ambiguous hybridization event : " << "  time = " << time;
			ss << std::endl << "Nodes : [ ";
			for(size_t iN=0; iN<eventNodes.size(); ++iN) {
				ss << eventNodes[iN]->getId() << " ,";
			}
			ss << "]";
			break;
		case POLYPLOID_DIAMOND:
			assert(!eventNodes.empty());
			ss << "Allopolyploid singleton : [ ";
			for(size_t iN=0; iN<eventNodes.size(); ++iN) {
				ss << eventNodes[iN]->getId() << " ,";
			}
			ss << "] - age = " << time;
			break;
		case POLYPLOID_TRIANGLE:
			ss << "Allopolyploid and one parent event : " << "  time = " << time;
			ss << std::endl << "Nodes : [ ";
			for(size_t iN=0; iN<eventNodes.size(); ++iN) {
				ss << eventNodes[iN]->getId() << " ,";
			}
			ss << "]";
			break;
		case NEW_POLYPLOID_TRIANGLE:
			ss << "Allopolyploid and two parents event : " << "  time = " << time;
			ss << std::endl << "Nodes : [ ";
			for(size_t iN=0; iN<eventNodes.size(); ++iN) {
				ss << eventNodes[iN]->getId() << " ,";
			}
			ss << "]";
			break;
		case FINAL_NODE_EVENT:
			assert(!eventNodes.empty() && eventNodes.size() == 1);
			ss << "Final node event (origin/root) : " << eventNodes.front()->getId() << " - age = " << time;
			break;
		case RESCALING_EVENT:
			ss << "Rescaling at time t=" << time << " for branches: ";
//			for(size_t iN=0; iN<eventNodes.size(); ++iN) ss << eventNodes[iN]->getEdgeToParent()->getId() << ", ";
			break;
		default:
			assert(false && "Unregistered event.");
			break;
	}

	return ss.str();
}


} /* namespace Scheduler */
} /* namespace Likelihood */
