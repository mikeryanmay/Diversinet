/*
 * EigenIntegrationKernel.cpp
 *
 *  Created on: Apr 17, 2023
 *      Author: mike
 */

#include "EigenIntegrationKernel.h"
#include "Models/IncModel.h"

namespace Likelihood {
namespace Kernels {
namespace CPU {

EigenIntegrationKernel::EigenIntegrationKernel(const size_t N_MAX_STATE_VECTOR, Models::ModelSharedPtr aPtrModel) :
	ptrModel(aPtrModel) {
}

EigenIntegrationKernel::~EigenIntegrationKernel() {
}


void EigenIntegrationKernel::operator() (const Likelihood::StateType::Vector::EigenState &x, Likelihood::StateType::Vector::EigenState &dxdt, double t) {
	doIntegrationStep(x, dxdt, t);
}

void EigenIntegrationKernel::doIntegrationStep(const Likelihood::StateType::Vector::EigenState &x, Likelihood::StateType::Vector::EigenState &dxdt, double t) {

	// get the transition matrix
	const Models::SpMat &Q = ptrModel->getTransitionRateMatrix(t);

	// get the probability elements
	Eigen::VectorXd &dpdt = dxdt.getStateProb();
	const Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	dpdt = Q * p;

}


} /* namespace CPU */
} /* namespace Kernels */
} /* namespace Likelihood */
