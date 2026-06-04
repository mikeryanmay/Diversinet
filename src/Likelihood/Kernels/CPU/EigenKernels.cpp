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

	// get the probability
	Eigen::VectorXd& probs = x.getStateProb();

	// set to probability from model
	probs = ptrModel->getInitialProbabilities(nNodes);

}

void EigenKernels::computeSpeciationEvent(double t, Likelihood::StateType::Vector::EigenState &x) {

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	ptrModel->applySpeciationEvent(p, t);

}

void EigenKernels::computeDirectionalTriangle(double t, Likelihood::StateType::Vector::EigenState &x) {

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	ptrModel->applyDirectionalTriangleEvent(p, t);

}

void EigenKernels::computeBidirectionalTriangle(double t, Likelihood::StateType::Vector::EigenState &x) {

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	ptrModel->applyBidirectionalTriangleEvent(p, t);

}

void EigenKernels::computeNewHybridTriangle(double t, Likelihood::StateType::Vector::EigenState &x) {

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	ptrModel->applyNewHybridTriangleEvent(p, t);

}

void EigenKernels::computeHybridDiamond(double t, Likelihood::StateType::Vector::EigenState &x) {

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	ptrModel->applyHybridDiamondEvent(p, t);

}

void EigenKernels::computePolyploidDiamond(double t, Likelihood::StateType::Vector::EigenState &x) {

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	ptrModel->applyPolyploidDiamondEvent(p, t);

}

void EigenKernels::computePolyploidTriangle(double t, Likelihood::StateType::Vector::EigenState &x) {

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	ptrModel->applyPolyploidTriangleEvent(p, t);

}

void EigenKernels::computeNewPolyploidTriangle(double t, Likelihood::StateType::Vector::EigenState &x) {

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	ptrModel->applyNewPolyploidTriangleEvent(p, t);

}

double EigenKernels::computeLogLikelihood(double t, Likelihood::StateType::Vector::EigenState &x) {

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// get the probability of the first element
	double logLikelihood = std::log(p[0]);

	// return
	return logLikelihood;
}

void EigenKernels::rescaleProbabilities(Likelihood::StateType::Vector::EigenState &x) {
	x.rescaleProbabilities();
}


} /* namespace CPU */
} /* namespace Kernels */
} /* namespace Likelihood */
