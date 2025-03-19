/*
 * DenseApproximator.h
 *
 *  Created on: Apr 13, 2023
 *      Author: mike
 */

#ifndef SRC_LIKELIHOOD_APPROXIMATOR_DEFAULTAPPROXIMATOR_H_
#define SRC_LIKELIHOOD_APPROXIMATOR_DEFAULTAPPROXIMATOR_H_

#include "BaseApproximator.h"
#include "Likelihood/StateTypes/Vector/EigenState.h"
#include "Likelihood/StateTypes/Vector/EigenStateOperations.hpp"
#include "Likelihood/Kernels/CPU/IncEigenKernels.h"

namespace Likelihood {
namespace Approximator {

class DefaultApproximator: public BaseApproximator {

	public:

		DefaultApproximator(Likelihood::Integrator::integrationScheme_t aIntScheme,
				            Scheduler::SchedulerSharedPtr aPtrScheduler,
						    Models::ModelSharedPtr aPtrModel,
							Conditions::conditionalProbability_t aConditionType);
		virtual ~DefaultApproximator();

		double approximateLogLikelihood();
		void setDefaultDeltaT(double aDeltaT);
		size_t getTotalNumberOfIntegrationSteps() const;

	private:

		const size_t N_MAX_STATE_VECTOR;

		typedef Likelihood::StateType::Vector::EigenState stateType_t;
		typedef Likelihood::StateType::Vector::EigenStateOperations operations_t;
		typedef Likelihood::Kernels::CPU::EigenKernels kernels_t;
		typedef Likelihood::Kernels::CPU::EigenIntegrationKernel intKernel_t;

		stateType_t probState;
		kernels_t kernels;
		intKernel_t intKernel;

		Likelihood::Integrator::Base<stateType_t, intKernel_t, operations_t>* ptrIntegrator;

		void doPreProcessingSteps();
		void doIntegrationStep(size_t iEdgesLayer);
		void doEventStep(size_t iEvent);
		void doPostProcessingSteps();
		void doReportState(double t);

};

} /* namespace Approximator */
} /* namespace Likelihood */

#endif /* SRC_LIKELIHOOD_APPROXIMATOR_DEFAULTAPPROXIMATOR_H_ */
