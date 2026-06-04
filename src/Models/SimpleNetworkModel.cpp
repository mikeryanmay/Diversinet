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
