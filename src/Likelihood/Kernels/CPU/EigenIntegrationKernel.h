/*
 * EigenIntegrationKernel.h
 *
 *  Created on: Apr 17, 2023
 *      Author: mike
 */

#ifndef SRC_LIKELIHOOD_KERNELS_CPU_EIGENINTEGRATIONKERNEL_H_
#define SRC_LIKELIHOOD_KERNELS_CPU_EIGENINTEGRATIONKERNEL_H_

#include "Likelihood/Scheduler/IncFwdScheduler.h"
#include "Likelihood/StateTypes/Vector/EigenState.h"
#include "Models/IncFwdModel.h"

namespace Likelihood {
namespace Kernels {
namespace CPU {

class EigenIntegrationKernel {

	public:

		EigenIntegrationKernel(const size_t N_MAX_STATE_VECTOR,
				               Models::ModelSharedPtr aPtrModel);
		~EigenIntegrationKernel();

		void operator() (const Likelihood::StateType::Vector::EigenState &x, Likelihood::StateType::Vector::EigenState &dxdt, double t);

	private:

		Models::ModelSharedPtr ptrModel;

		void doIntegrationStep(const Likelihood::StateType::Vector::EigenState &x, Likelihood::StateType::Vector::EigenState &dxdt, double t);


};

} /* namespace CPU */
} /* namespace Kernels */
} /* namespace Likelihood */

#endif /* SRC_LIKELIHOOD_KERNELS_CPU_EIGENINTEGRATIONKERNEL_H_ */
