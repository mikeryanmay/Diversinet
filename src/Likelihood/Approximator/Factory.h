/*
 * Factory.h
 *
 *  Created on: Apr 13, 2023
 *      Author: mike
 */

#ifndef SRC_LIKELIHOOD_APPROXIMATOR_FACTORY_H_
#define SRC_LIKELIHOOD_APPROXIMATOR_FACTORY_H_

#include "BaseApproximator.h"
#include "Models/IncFwdModel.h"
#include "Likelihood/Scheduler/IncFwdScheduler.h"
#include "Likelihood/Integrators/IntegratorFactory.h"

namespace Likelihood {
namespace Approximator {

typedef boost::shared_ptr<BaseApproximator> ApproximatorSharedPtr;

class Factory {

	public:

		static ApproximatorSharedPtr createDefaultApproximator(Likelihood::Integrator::integrationScheme_t aIntScheme,
                                                               Scheduler::SchedulerSharedPtr aPtrScheduler,
															   Models::ModelSharedPtr aPtrModel,
															   Conditions::conditionalProbability_t aConditionType);

	private:

		Factory();
		~Factory();

};

} /* namespace Approximator */
} /* namespace Likelihood */

#endif /* SRC_LIKELIHOOD_APPROXIMATOR_FACTORY_H_ */
