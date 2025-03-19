/*
 * Factory.cpp
 *
 *  Created on: Apr 13, 2023
 *      Author: mike
 */

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <Likelihood/Approximator/DefaultApproximator.h>

#include "Factory.h"
#include "Likelihood/Approximator/BaseApproximator.h"

namespace Likelihood {
namespace Approximator {

Factory::Factory() {
}

Factory::~Factory() {
}

ApproximatorSharedPtr Factory::createDefaultApproximator(Likelihood::Integrator::integrationScheme_t aIntScheme,
                                                       Scheduler::SchedulerSharedPtr aPtrScheduler,
													   Models::ModelSharedPtr aPtrModel,
													   Conditions::conditionalProbability_t aConditionType) {
	// create the approximator
	boost::shared_ptr<DefaultApproximator> ptrDense = boost::make_shared<DefaultApproximator>(aIntScheme, aPtrScheduler, aPtrModel, aConditionType);
	ApproximatorSharedPtr approxPtr = ptrDense; // @suppress("Invalid arguments")
	return approxPtr;
}

} /* namespace Approximator */
} /* namespace Likelihood */
