/*
 * BaseScheduler.h
 *
 *  Created on: Apr 11, 2023
 *      Author: mike
 */

#ifndef SRC_LIKELIHOOD_SCHEDULER_BASESCHEDULER_H_
#define SRC_LIKELIHOOD_SCHEDULER_BASESCHEDULER_H_

#include <boost/smart_ptr/shared_ptr.hpp>
#include <list>
#include <vector>

#include "../../Data/Structure/IncFwdNetworkStructure.h"
#include "EventType.h"

namespace Likelihood {
namespace Scheduler {

class Event;

class BaseScheduler {

	public:
		static const double MAX_SEGMENT_SIZE_WITHOUT_RESCALING;

	public:

		BaseScheduler(NS::NetworkSharedPtr aPtrNetwork);
		virtual ~BaseScheduler();

		NS::NetworkSharedPtr getPtrNetwork() const;
		const std::vector<Event*>& getEvents() const;

		bool hasBeenUpdated() const;
		void clearHasBeenUpdatedFlag();

		size_t getNumEdgesForLayer(size_t iLayer);

	protected:

		bool updated = true;
		NS::NetworkSharedPtr ptrNetwork;
		std::vector<Event*> events;

		typedef std::list<NS::EdgeSharedPtr> edgesList_t;
		typedef edgesList_t::iterator itEdgesList_t;
		std::vector<edgesList_t> layeredEdges;

	private:

		void clearEvents();
		void initEvents();
		std::vector<NS::NodeSharedPtr> defineNextEdgesLayerAndEvent(Event *lastEvent);

		eventType_t eventTypeFromEventNodes(std::vector<NS::NodeSharedPtr> eventNodes);

};

} /* namespace Scheduler */
} /* namespace Likelihood */

#endif /* SRC_LIKELIHOOD_SCHEDULER_BASESCHEDULER_H_ */
