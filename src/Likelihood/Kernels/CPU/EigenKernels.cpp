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
	double &sampleProb  = ptrModel->ptrParameters->rho;
	double noSampleProb = 1.0 - sampleProb;

	// get reference to probabilities
	Eigen::VectorXd& probs = x.getStateProb();

	// compute the base probability
	double baseProb = std::pow(sampleProb, (double)nNodes);

	// initialize each probability
	for(size_t iL = 0; iL < probs.size(); ++iL) {
		probs(iL) = baseProb * std::pow(noSampleProb, (double)iL);
	}

}

void EigenKernels::computeSpeciationEvent(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO

	// get the transition matrix
	const Models::SpMat &P = ptrModel->getSpeciationEventMatrix(t);

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	p = P * p;

}

void EigenKernels::computeDirectionalTriangle(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO

	// get the transition matrix
	const Models::SpMat &P = ptrModel->getDirectionalTriangleEventMatrix(t);

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	p = P * p;

}

void EigenKernels::computeBidirectionalTriangle(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO

	// get the transition matrix
	const Models::SpMat &P = ptrModel->getBidirectionalTriangleEventMatrix(t);

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	p = P * p;

}

void EigenKernels::computeNewHybridTriangle(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO

	// get the transition matrix
	const Models::SpMat &P = ptrModel->getNewHybridTriangleEventMatrix(t);

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	p = P * p;

}

void EigenKernels::computeHybridDiamond(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO

	// get the transition matrix
	const Models::SpMat &P = ptrModel->getHybridDiamondEventMatrix(t);

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	p = P * p;

}

void EigenKernels::computePolyploidDiamond(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO

	// get the transition matrix
	const Models::SpMat &P = ptrModel->getPolyploidTriangleEventMatrix(t);

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	p = P * p;

}

void EigenKernels::computePolyploidTriangle(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO

	// get the transition matrix
	const Models::SpMat &P = ptrModel->getNewPolyploidTriangleEventMatrix(t);

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	p = P * p;

}

void EigenKernels::computeNewPolyploidTriangle(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO

	// get the transition matrix
	const Models::SpMat &P = ptrModel->getPolyploidDiamondEventMatrix(t);

	// get the probability elements
	Eigen::VectorXd &p = x.getStateProb();

	// do the operation
	p = P * p;

}

double EigenKernels::computeLogLikelihood(double t, Likelihood::StateType::Vector::EigenState &x) {
	// TODO
	return 0.0;
}

void EigenKernels::rescaleProbabilities(Likelihood::StateType::Vector::EigenState &x) {
	x.rescaleProbabilities();
}


} /* namespace CPU */
} /* namespace Kernels */
} /* namespace Likelihood */
