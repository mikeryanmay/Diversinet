/*
 * EventType.h
 *
 *  Created on: Aug 29, 2019
 *      Author: xaviermeyer
 */

#ifndef LIKELIHOOD_SCHEDULER_EVENTTYPE_H_
#define LIKELIHOOD_SCHEDULER_EVENTTYPE_H_

namespace Likelihood {
namespace Scheduler {

//typedef enum {
//	PRESENT_TIME_EVENT,               // sampling event at the present
//	SPECIATION_EVENT,                 // a lineage splits in two, both have descendants
//	ASYMMETRICAL_HYBRIDIZATION_EVENT, // i) one lineage hybridizes into another, both have descendants; ii) two lineages perform hybrid speciation then one parent goes extinct
//	SYMMETRICAL_HYBRIDIZATION_EVENT,  // two lineages hybridize into each other, both have descendants
//	HYBRID_SPECIATION_EVENT,          // two lineages form a third, all have descendants
//	AMBIGUOUS_HYBRIDIZATION_EVENT,    // i) asymmetrical hybridization where donor goes extinct; ii) symmetrical hybridization where one lineage goes extinct; iii) hybrid speciation where both parents go extinct
//	ALLOPOLYPLOIDIZATION_EVENT_TWO,   // two lineages form an allopolyploid descendant, one parent goes extinct
//	ALLOPOLYPLOIDIZATION_EVENT_THREE, // two lineages form an allopolyploid descendant, all three have descendants
//	ALLOPOLYPLOIDIZATION_EVENT_ONE,   // two lineages form an allopolyploid descendant, both parents goes extinct
//	FINAL_NODE_EVENT,                 // the last node in the network (the root or origin)
//	RESCALING_EVENT                   // a pseudo-event to rescale likelihood
//} eventType_t;

typedef enum {
	PRESENT_TIME_EVENT,
	SPECIATION_EVENT,
	DIRECTIONAL_TRIANGLE,
	BIDIRECTIONAL_TRIANGLE,
	NEW_HYBRID_TRIANGLE,
	HYBRID_DIAMOND,
	POLYPLOID_TRIANGLE,
	NEW_POLYPLOID_TRIANGLE,
	POLYPLOID_DIAMOND,
	FINAL_NODE_EVENT,
	RESCALING_EVENT
} eventType_t;

}
}

#endif /* LIKELIHOOD_SCHEDULER_EVENTTYPE_H_ */
