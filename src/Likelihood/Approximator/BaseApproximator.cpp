/*
 * BaseApproximator.cpp
 *
 *  Created on: Apr 13, 2023
 *      Author: mike
 */

#include "BaseApproximator.h"
#include "Likelihood/Scheduler/Event.h"
#include "Likelihood/Scheduler/BaseScheduler.h"
#include "Likelihood/Integrators/IntegratorFactory.h"
#include "Models/IncModel.h"

namespace Likelihood {
namespace Approximator {

const double BaseApproximator::DEFAULT_DELTA_T = 5.E-1;
const double BaseApproximator::DEFAULT_ABS_TOLERANCE = 1.E-7;
const double BaseApproximator::DEFAULT_REL_TOLERANCE = 1.E-7;

BaseApproximator::BaseApproximator(Likelihood::Integrator::integrationScheme_t aIntScheme,
                                   Scheduler::SchedulerSharedPtr aPtrScheduler,
								   Models::ModelSharedPtr aPtrModel) :
								   intScheme(aIntScheme),
								   ptrScheduler(aPtrScheduler),
								   ptrModel(aPtrModel),
								   deltaT(DEFAULT_DELTA_T) {
}

BaseApproximator::~BaseApproximator() {
}

double BaseApproximator::approximateLikelihood() {
	return exp(approximateLogLikelihood());
}

const std::vector<double>& BaseApproximator::getIntegrationTimes() const{
	return integrationTimes;
}

bool BaseApproximator::areEventsPossible() {

	// Check the event validity
	for(size_t iE=0; iE<ptrScheduler->getEvents().size(); ++iE) {
		if(!ptrScheduler->getEvents()[iE]->isEventPossible()) { // If this event is impossible
			return false; // We are done
		}
	}

	// All events are possible
	return true;
}

} /* namespace Approximator */
} /* namespace Likelihood */
