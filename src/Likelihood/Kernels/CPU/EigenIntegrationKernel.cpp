/*
 * EigenIntegrationKernel.cpp
 *
 *  Created on: Apr 17, 2023
 *      Author: mike
 */

#include "EigenIntegrationKernel.h"

namespace Likelihood {
namespace Kernels {
namespace CPU {

EigenIntegrationKernel::EigenIntegrationKernel(const size_t N_MAX_STATE_VECTOR,
        Models::ModelSharedPtr aPtrModel) :
        	ptrModel(aPtrModel) {
}

EigenIntegrationKernel::~EigenIntegrationKernel() {
}

void EigenIntegrationKernel::operator() (const Likelihood::StateType::Vector::EigenState &x, Likelihood::StateType::Vector::EigenState &dxdt, double t) {
	doIntegrationStep(x, dxdt, t);
}

void EigenIntegrationKernel::doIntegrationStep(const Likelihood::StateType::Vector::EigenState &x, Likelihood::StateType::Vector::EigenState &dxdt, double t) {
	// TODO: implement dxdt
}


} /* namespace CPU */
} /* namespace Kernels */
} /* namespace Likelihood */
