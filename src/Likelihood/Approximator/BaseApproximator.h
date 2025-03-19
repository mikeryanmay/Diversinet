/*
 * BaseApproximator.h
 *
 *  Created on: Apr 13, 2023
 *      Author: mike
 */

#ifndef SRC_LIKELIHOOD_APPROXIMATOR_BASEAPPROXIMATOR_H_
#define SRC_LIKELIHOOD_APPROXIMATOR_BASEAPPROXIMATOR_H_

#include <vector>
#include <boost/smart_ptr/shared_ptr.hpp>

#include "Models/IncFwdModel.h"
#include "Likelihood/Scheduler/IncFwdScheduler.h"
#include "Likelihood/Integrators/IntegratorFactory.h"
#include "Likelihood/ConditionTypes/ConditionType.h"

namespace Likelihood {
namespace Approximator {

class BaseApproximator {

	public:
		BaseApproximator(Likelihood::Integrator::integrationScheme_t aIntScheme,
				         Scheduler::SchedulerSharedPtr aPtrScheduler,
						 Models::ModelSharedPtr aPtrModel,
						 Conditions::conditionalProbability_t aConditionType);
		virtual ~BaseApproximator();

		double approximateLikelihood();
		virtual double approximateLogLikelihood() = 0;

		virtual const std::vector<double>& getIntegrationTimes() const;
		virtual void setDefaultDeltaT(double aDeltaT) = 0;
		virtual size_t getTotalNumberOfIntegrationSteps() const = 0;

	public:

		static const double DEFAULT_DELTA_T, DEFAULT_ABS_TOLERANCE, DEFAULT_REL_TOLERANCE;

	protected:

		double deltaT, logLikelihood = 0.0, scalingFactor = 0.0;
		Likelihood::Integrator::integrationScheme_t intScheme;
		Likelihood::Conditions::conditionalProbability_t conditionType;
		Models::ModelSharedPtr ptrModel;
		Scheduler::SchedulerSharedPtr ptrScheduler;
		std::vector<double> integrationTimes;

		bool areEventsPossible();

};

} /* namespace Approximator */
} /* namespace Likelihood */

#endif /* SRC_LIKELIHOOD_APPROXIMATOR_BASEAPPROXIMATOR_H_ */
