/*
 * EigenKernels.h
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#ifndef SRC_LIKELIHOOD_KERNELS_CPU_EIGENKERNELS_H_
#define SRC_LIKELIHOOD_KERNELS_CPU_EIGENKERNELS_H_

#include "Data/Structure/IncFwdNetworkStructure.h"
#include "Likelihood/StateTypes/Vector/EigenState.h"
#include "Models/IncFwdModel.h"

namespace Likelihood {
namespace Kernels {
namespace CPU {

class EigenKernels {

	public:

		EigenKernels(Models::ModelSharedPtr aPtrModel);
		~EigenKernels();

		void setInitialCondition(const std::vector<Data::Structure::NodeSharedPtr>& extantNodes, Likelihood::StateType::Vector::EigenState &x);
		void computeSpeciationEvent(double t, Likelihood::StateType::Vector::EigenState &x);
		void computeDirectionalTriangle(double t, Likelihood::StateType::Vector::EigenState &x);
		void computeBidirectionalTriangle(double t, Likelihood::StateType::Vector::EigenState &x);
		void computeNewHybridTriangle(double t, Likelihood::StateType::Vector::EigenState &x);
		void computeHybridDiamond(double t, Likelihood::StateType::Vector::EigenState &x);
		void computePolyploidDiamond(double t, Likelihood::StateType::Vector::EigenState &x);
		void computePolyploidTriangle(double t, Likelihood::StateType::Vector::EigenState &x);
		void computeNewPolyploidTriangle(double t, Likelihood::StateType::Vector::EigenState &x);
		void rescaleProbabilities(Likelihood::StateType::Vector::EigenState &x);
		double computeLogLikelihood(double t, Likelihood::StateType::Vector::EigenState &x);

	private:

		Models::ModelSharedPtr ptrModel;

};

} /* namespace CPU */
} /* namespace Kernels */
} /* namespace Likelihood */

#endif /* SRC_LIKELIHOOD_KERNELS_CPU_EIGENKERNELS_H_ */
