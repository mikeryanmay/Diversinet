/*
 * Event.h
 *
 *  Created on: Apr 11, 2023
 *      Author: mike
 */

#ifndef SRC_LIKELIHOOD_SCHEDULER_EVENT_H_
#define SRC_LIKELIHOOD_SCHEDULER_EVENT_H_

#include <string>
#include <vector>
#include <boost/smart_ptr/shared_ptr.hpp>

#include "EventType.h"

namespace Data {
namespace Structure {
	class Node;
	typedef boost::shared_ptr<Node> NodeSharedPtr;
}
}

namespace Likelihood {
namespace Scheduler {

namespace NS = ::Data::Structure;

class Event {

	public:

		Event(eventType_t anEventType, double atTime);
		Event(eventType_t anEventType, NS::NodeSharedPtr aNode);
		Event(eventType_t anEventType, const std::vector<NS::NodeSharedPtr> &aNodes);
		Event(eventType_t anEventType, double atTime, const std::vector<NS::NodeSharedPtr> &aNodes);

		virtual ~Event();

		void updateEvent(eventType_t aEventType);
		bool checkEvent(eventType_t aEventType) const;
		bool isNodeEvent() const;
		eventType_t getEvent() const;

		void setTime(double aTime);
		double getTime() const;
		const std::vector<NS::NodeSharedPtr>& getNodes() const;

		bool isEventPossible() const;

		std::string toString() const;

	private:

		eventType_t eventType;
		double time;
		std::vector<NS::NodeSharedPtr> eventNodes;

};

} /* namespace Scheduler */
} /* namespace Likelihood */

#endif /* SRC_LIKELIHOOD_SCHEDULER_EVENT_H_ */
