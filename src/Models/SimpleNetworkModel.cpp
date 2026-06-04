/*
 * SimpleNetworkModel.cpp
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#include <iostream>

#include "SimpleNetworkModel.h"
#include "Parameters/IncParameterContainer.h"

namespace Models {

SimpleNetworkModel::SimpleNetworkModel(Parameters::ContainerSharedPtr aPtrParameter, size_t aKMax = 1024) :
  BaseModel(aPtrParameter, aKMax) {
}

SimpleNetworkModel::~SimpleNetworkModel() {
}

void SimpleNetworkModel::resizeIfKmaxChanged(SpMat &matrix) {
	if ((size_t)matrix.rows() != Kmax || (size_t)matrix.cols() != Kmax) {
		matrix.resize(Kmax, Kmax);
	}
}

void SimpleNetworkModel::resizeEventScratch(size_t size) {
	if ((size_t)eventScratch.size() != size) {
		eventScratch.resize(size);
	}
}


const Eigen::VectorXd& SimpleNetworkModel::getInitialProbabilities(size_t numLineages) {
	updateInitialProbabilities(numLineages);
	return initialProbabilities;
}

const SpMat& SimpleNetworkModel::getTransitionRateMatrix(double t) {
	updateRateMatrix();
	return transitionRateMatrix;
}

const SpMat& SimpleNetworkModel::getSpeciationEventMatrix(double t) {
	updateSpeciationEventMatrix();
	return speciationEventMatrix;
}

const SpMat& SimpleNetworkModel::getDirectionalTriangleEventMatrix(double t) {
	updateDirectionalTriangleEventMatrix();
	return directionalTriangleEventMatrix;
}

const SpMat& SimpleNetworkModel::getBidirectionalTriangleEventMatrix(double t) {
	updateBidirectionalTriangleEventMatrix();
	return bidirectionalTriangleEventMatrix;
}

const SpMat& SimpleNetworkModel::getNewHybridTriangleEventMatrix(double t) {
	updateNewHybridTriangleEventMatrix();
	return newHybridTriangleEventMatrix;
}

const SpMat& SimpleNetworkModel::getHybridDiamondEventMatrix(double t) {
	updateHybridDiamondEventMatrix();
	return hybridDiamondEventMatrix;
}

const SpMat& SimpleNetworkModel::getPolyploidTriangleEventMatrix(double t) {
	updatePolyploidTriangleEventMatrix();
	return polyploidTriangleEventMatrix;
}

const SpMat& SimpleNetworkModel::getNewPolyploidTriangleEventMatrix(double t) {
	updateNewPolyploidTriangleEventMatrix();
	return newPolyploidTriangleEventMatrix;
}

const SpMat& SimpleNetworkModel::getPolyploidDiamondEventMatrix(double t) {
	updatePolyploidDiamondEventMatrix();
	return polyploidDiamondEventMatrix;
}

void SimpleNetworkModel::computeTransitionRateAction(Eigen::VectorXd &dxdt, const Eigen::VectorXd &p, double t) {

	if (dxdt.size() != p.size()) {
		dxdt.resize(p.size());
	}

	const double &lambda = ptrParameters->lambda;
	const double &mu     = ptrParameters->mu;
	const double &eta    = ptrParameters->eta;
	const double &zeta   = ptrParameters->zeta;
	const double &nu     = ptrParameters->nu;
	const double &psi    = ptrParameters->psi;

	const double s = (double)numLineages;
	const double lambdaPlusMu    = lambda + mu;
	const double etaPlusZeta     = eta + zeta;
	const double zetaPlusHalfEta = zeta + 0.5 * eta;
	const double nuPlusPsi       = nu + psi;
	const size_t nStates = (size_t)p.size();

	for (size_t iU = 0; iU < nStates; ++iU) {
		const double u = (double)iU;
		const double n = s + u;

		const double diag = -(n * lambdaPlusMu + s * (s - 1) * 0.5 * etaPlusZeta + s * u * zetaPlusHalfEta + n * (n - 1) * 0.5 * nuPlusPsi);

		double value = diag * p(iU);
		if (iU < nStates - 1) {
			const double up = (u + 2 * s) * lambda + n * (n - 1) * 0.5 * nuPlusPsi;
			value += up * p(iU + 1);
		}
		if (iU > 0) {
			const double down = u * mu;
			value += down * p(iU - 1);
		}

		dxdt(iU) = value;
	}

}

void SimpleNetworkModel::applySpeciationEvent(Eigen::VectorXd &p, double t) {
	p *= ptrParameters->lambda;
}

void SimpleNetworkModel::applyDirectionalTriangleEvent(Eigen::VectorXd &p, double t) {

	resizeEventScratch((size_t)p.size());

	const double diag = 0.5 * ptrParameters->eta;
	const double up   = ptrParameters->nu;
	const size_t nStates = (size_t)p.size();

	for (size_t iU = 0; iU < nStates; ++iU) {
		eventScratch(iU) = diag * p(iU);
		if (iU < nStates - 1) {
			eventScratch(iU) += up * p(iU + 1);
		}
	}

	p = eventScratch;

}

void SimpleNetworkModel::applyBidirectionalTriangleEvent(Eigen::VectorXd &p, double t) {
	p *= ptrParameters->zeta;
}

void SimpleNetworkModel::applyNewHybridTriangleEvent(Eigen::VectorXd &p, double t) {
	p *= ptrParameters->nu;
}

void SimpleNetworkModel::applyHybridDiamondEvent(Eigen::VectorXd &p, double t) {

	resizeEventScratch((size_t)p.size());

	const double up  = ptrParameters->eta + 2.0 * ptrParameters->zeta;
	const double up2 = ptrParameters->nu;
	const size_t nStates = (size_t)p.size();

	for (size_t iU = 0; iU < nStates; ++iU) {
		double value = 0.0;
		if (iU < nStates - 1) {
			value += up * p(iU + 1);
		}
		if (iU < nStates - 2) {
			value += up2 * p(iU + 2);
		}
		eventScratch(iU) = value;
	}

	p = eventScratch;

}

void SimpleNetworkModel::applyPolyploidTriangleEvent(Eigen::VectorXd &p, double t) {

	resizeEventScratch((size_t)p.size());

	const double up = ptrParameters->psi;
	const size_t nStates = (size_t)p.size();

	for (size_t iU = 0; iU < nStates; ++iU) {
		eventScratch(iU) = iU < nStates - 1 ? up * p(iU + 1) : 0.0;
	}

	p = eventScratch;

}

void SimpleNetworkModel::applyNewPolyploidTriangleEvent(Eigen::VectorXd &p, double t) {
	p *= ptrParameters->psi;
}

void SimpleNetworkModel::applyPolyploidDiamondEvent(Eigen::VectorXd &p, double t) {

	resizeEventScratch((size_t)p.size());

	const double up2 = ptrParameters->psi;
	const size_t nStates = (size_t)p.size();

	for (size_t iU = 0; iU < nStates; ++iU) {
		eventScratch(iU) = iU < nStates - 2 ? up2 * p(iU + 2) : 0.0;
	}

	p = eventScratch;

}

void SimpleNetworkModel::updateInitialProbabilities(size_t initialNumberOfLineages) {

	if (needsUpdateInitialProbabilities) {

		initialProbabilities.resize(Kmax);

		// sampling fraction parameter
		double &sampleProb  = ptrParameters->rho;
		double noSampleProb = 1.0 - sampleProb;

		// compute the base probability
		double baseProb = std::pow(sampleProb, (double)initialNumberOfLineages);

		// initialize each probability
		for(size_t iL = 0; iL < initialProbabilities.size(); ++iL) {
			initialProbabilities(iL) = baseProb * std::pow(noSampleProb, (double)iL);
		}

		// mark as clean
		needsUpdateInitialProbabilities = false;

	}

}

void SimpleNetworkModel::updateRateMatrix() {

	// only update if necessary
	if (needsUpdateRateMatrix) {

		resizeIfKmaxChanged(transitionRateMatrix);

		// get parameters
		const double &lambda = ptrParameters->lambda; // speciation rate
		const double &mu     = ptrParameters->mu;     // extinction rate
		const double &eta    = ptrParameters->eta;    // asymmetrical hybridization rate
		const double &zeta   = ptrParameters->zeta;   // symmetrical hybridization rate
		const double &nu     = ptrParameters->nu;     // hybrid speciation rate
		const double &psi    = ptrParameters->psi;    // allopolyploidization rate

		// number of sampled lineages
		double s = (double)numLineages;

		// parameter coefficients
		double lambdaPlusMu    = lambda + mu;
		double etaPlusZeta     = eta + zeta;
		double zetaPlusHalfEta = zeta + 0.5 * eta;
		double nuPlusPsi       = nu + psi;

		// fill in values
		double u, n;
		for (size_t iU = 0; iU < Kmax; ++iU) {

			u = double(iU); // number of unsampled lineages
			n = s + u;      // total number of lineages

			// diagonal value
			transitionRateMatrix.coeffRef(iU, iU) = -(n * lambdaPlusMu + s * (s - 1) * 0.5 * etaPlusZeta + s * u * zetaPlusHalfEta + n * (n - 1) * 0.5 * nuPlusPsi);

			// up value (only if we can transition up
			if ( iU < (Kmax - 1) ) {
				transitionRateMatrix.coeffRef(iU, iU + 1) = (u + 2 * s) * lambda + n * (n - 1) * 0.5 * nuPlusPsi;
			}

			// down value (only if u > 0)
			if ( iU > 0 ) {
				transitionRateMatrix.coeffRef(iU, iU - 1) = u * mu;
			}

		}

		// mark as clean
		needsUpdateRateMatrix = false;

	}

}

void SimpleNetworkModel::updateSpeciationEventMatrix() {

	// only update if necessary
	if (needsUpdateSpeciationEventMatrix) {

		resizeIfKmaxChanged(speciationEventMatrix);

		// get parameters
		const double &lambda = ptrParameters->lambda; // speciation rate

		// fill in values
		for (size_t iU = 0; iU < Kmax; ++iU) {

			// diagonal value
//			speciationEventMatrix.coeffRef(iU, iU) = 2.0 * lambda;
			speciationEventMatrix.coeffRef(iU, iU) = lambda;

		}

		// mark as clean
		needsUpdateSpeciationEventMatrix = false;
	}

}

void SimpleNetworkModel::updateDirectionalTriangleEventMatrix() {

	// only update if necessary
	if (needsUpdateDirectionalTriangleEventMatrix) {

		resizeIfKmaxChanged(directionalTriangleEventMatrix);

		// get parameters
		const double &eta = ptrParameters->eta; // asymmetrical hybridization rate
		const double &nu  = ptrParameters->nu;  // hybrid speciation rate

		// fill in values
		for (size_t iU = 0; iU < Kmax; ++iU) {

			// diagonal value
			directionalTriangleEventMatrix.coeffRef(iU, iU) = 0.5 * eta;

			// up value (only if we can transition up
			if ( iU < (Kmax - 1) ) {
				directionalTriangleEventMatrix.coeffRef(iU, iU + 1) = nu;
			}

		}

		// mark as clean
		needsUpdateDirectionalTriangleEventMatrix = false;
	}

}

void SimpleNetworkModel::updateBidirectionalTriangleEventMatrix() {

	// only update if necessary
	if (needsUpdateBidirectionalTriangleEventMatrix) {

		resizeIfKmaxChanged(bidirectionalTriangleEventMatrix);

		// get parameters
		const double &zeta = ptrParameters->zeta; // symmetrical hybridization rate

		// fill in values
		for (size_t iU = 0; iU < Kmax; ++iU) {

			// diagonal value
			bidirectionalTriangleEventMatrix.coeffRef(iU, iU) = zeta;

		}

		// std::cout << "BIDIRECTIONAL TRIANGLE" << std::endl;

		// mark as clean
		needsUpdateBidirectionalTriangleEventMatrix = false;
	}

}

void SimpleNetworkModel::updateNewHybridTriangleEventMatrix() {

	// only update if necessary
	if (needsUpdateNewHybridTriangleEventMatrix) {

		resizeIfKmaxChanged(newHybridTriangleEventMatrix);

		// get parameters
		const double &nu = ptrParameters->nu;     // hybrid speciation rate

		// fill in values
		for (size_t iU = 0; iU < Kmax; ++iU) {

			// diagonal value
			newHybridTriangleEventMatrix.coeffRef(iU, iU) = nu;

		}

		// mark as clean
		needsUpdateNewHybridTriangleEventMatrix = false;
	}

}

void SimpleNetworkModel::updateHybridDiamondEventMatrix() {

	// only update if necessary
	if (needsUpdateHybridDiamondEventMatrix) {

		resizeIfKmaxChanged(hybridDiamondEventMatrix);

		// get parameters
		const double &eta  = ptrParameters->eta;  // asymmetrical hybridization rate
		const double &zeta = ptrParameters->zeta; // symmetrical hybridization rate
		const double &nu   = ptrParameters->nu;   // hybrid speciation rate

		// fill in values
		for (size_t iU = 0; iU < Kmax; ++iU) {

			// no diagonal value
			hybridDiamondEventMatrix.coeffRef(iU, iU) = 0.0;

			// up value (only if we can transition up)
			if ( iU < (Kmax - 1) ) {
				hybridDiamondEventMatrix.coeffRef(iU, iU + 1) = eta + 2.0 * zeta;
				// hybridDiamondEventMatrix.coeffRef(iU, iU + 1) = eta + zeta;
			}

			// std::cout << "HYBRID DIAMOND" << std::endl;

			// up value (only if we can transition up twice)
			if ( iU < (Kmax - 2) ) {
				hybridDiamondEventMatrix.coeffRef(iU, iU + 2) = nu;
			}

		}

		// mark as clean
		needsUpdateHybridDiamondEventMatrix = false;
	}

}

void SimpleNetworkModel::updatePolyploidTriangleEventMatrix() {

	// only update if necessary
	if (needsUpdatePolyploidTriangleEventMatrix) {

		resizeIfKmaxChanged(polyploidTriangleEventMatrix);

		// get parameters
		const double &psi = ptrParameters->psi;    // allopolyploidization rate

		// fill in values
		for (size_t iU = 0; iU < Kmax; ++iU) {

			// no diagonal value
			polyploidTriangleEventMatrix.coeffRef(iU, iU) = 0.0;

			// up value (only if we can transition up)
			if ( iU < (Kmax - 1) ) {
				polyploidTriangleEventMatrix.coeffRef(iU, iU + 1) = psi;
			}

		}

		// mark as clean
		needsUpdatePolyploidTriangleEventMatrix = false;
	}

}

void SimpleNetworkModel::updateNewPolyploidTriangleEventMatrix() {

	// only update if necessary
	if (needsUpdateNewPolyploidTriangleEventMatrix) {

		resizeIfKmaxChanged(newPolyploidTriangleEventMatrix);

		// get parameters
		const double &psi = ptrParameters->psi; // allopolyploidization rate

		// fill in values
		for (size_t iU = 0; iU < Kmax; ++iU) {

			// diagonal value
			newPolyploidTriangleEventMatrix.coeffRef(iU, iU) = psi;

		}

		// mark as clean
		needsUpdateNewPolyploidTriangleEventMatrix = false;
	}

}

void SimpleNetworkModel::SimpleNetworkModel::updatePolyploidDiamondEventMatrix() {

	// only update if necessary
	if (needsUpdatePolyploidDiamondEventMatrix) {

		resizeIfKmaxChanged(polyploidDiamondEventMatrix);

		const double &psi = ptrParameters->psi;    // allopolyploidization rate

		// fill in values
		for (size_t iU = 0; iU < Kmax; ++iU) {

			// no diagonal value
			polyploidDiamondEventMatrix.coeffRef(iU, iU) = 0.0;

			// up value (only if we can transition up twice)
			if ( iU < (Kmax - 2) ) {
				polyploidDiamondEventMatrix.coeffRef(iU, iU + 2) = psi;
			}

		}

		// mark as clean
		needsUpdatePolyploidDiamondEventMatrix = false;
	}

}


} /* namespace Models */
