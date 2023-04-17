/*
 * EigenKernels.cpp
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#include "EigenKernels.h"
#include "Data/Structure/IncNetworkStructure.h"
#include "Likelihood/StateTypes/Utils.h"
#include "Likelihood/Scheduler/IncScheduler.h"
#include "Models/IncModel.h"
#include "Parameters/IncParameterContainer.h"

namespace Likelihood {
namespace Kernels {
namespace CPU {

EigenKernels::EigenKernels(Models::ModelSharedPtr aPtrModel) : ptrModel(aPtrModel) {
}

EigenKernels::~EigenKernels() {
}

void EigenKernels::setInitialCondition(const std::vector<Data::Structure::NodeSharedPtr>& extantNodes,
		Likelihood::StateType::Vector::EigenState &x) {

	// number of nodes
	size_t nNodes = extantNodes.size();

	// sampling fraction parameter
	double &rho = ptrModel->ptrParameters->rho;

	// compute the base probability
	double baseProb = std::pow(rho, (double)nNodes);

	// initialize each probability
	for(size_t iL = 0; iL < x.size(); ++iL) {
		// TODO
	}


}

void EigenKernels::computeSpeciationEvent(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO
}

void EigenKernels::computeAsymmetricalHybridizationEvent(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO
}

void EigenKernels::computeSymmetricalHybridizationEvent(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO
}

void EigenKernels::computeHybridSpeciationEvent(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO
}

void EigenKernels::computeAmbiguousHybridizationEvent(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO
}

void EigenKernels::computeAllopolyploidizationEventOne(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO
}

void EigenKernels::computeAllopolyploidizationEventTwo(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO
}

void EigenKernels::computeAllopolyploidizationEventThree(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO
}

double EigenKernels::computeLogLikelihood(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO
	return 0.0;
}

} /* namespace CPU */
} /* namespace Kernels */
} /* namespace Likelihood */
