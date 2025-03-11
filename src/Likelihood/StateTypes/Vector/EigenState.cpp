/*
 * EigenState.cpp
 *
 *  Created on: Apr 13, 2023
 *      Author: mike
 */

#include "EigenState.h"
#include "Utils/MemoryPool/EigenCPU.h"
#include "../Utils.h"

namespace Likelihood {
namespace StateType {
namespace Vector {

EigenState::EigenState() : scaling(0.0), probVec(NULL) {
}

EigenState::EigenState(const EigenState &aEigenState) :
		scaling(aEigenState.scaling), probVec(NULL) {

	if(aEigenState.probVec != NULL) {
		initMemory();
		probVec->vector = aEigenState.probVec->vector;
	} else {
		probVec = NULL;
	}

}

EigenState::~EigenState() {

	if(probVec != NULL) {
		Utils::MemoryPool::eigenCPU().threadSafeFreeVector(probVec);
		probVec = NULL;
	}

}

void EigenState::resize() {

	// Do nothing instead?
	if(probVec == NULL) {
		allocateVecProb();
	}
}

void EigenState::allocateVecProb() {
	assert(probVec == NULL);
	if(probVec == NULL) {
		probVec = Utils::MemoryPool::eigenCPU().threadSafeAllocateVector();
	}
}

void EigenState::removeVecProb() {
	assert(probVec != NULL);
	if(probVec != NULL) {
		Utils::MemoryPool::eigenCPU().threadSafeFreeVector(probVec);
		probVec = NULL;
	}
}


Eigen::VectorXd& EigenState::getStateProb() {
	if(probVec == NULL) {
		probVec = Utils::MemoryPool::eigenCPU().threadSafeAllocateVector();
	}
	return probVec->vector;
}

const Eigen::VectorXd& EigenState::getStateProb() const {
	assert(probVec != NULL);
	return probVec->vector;
}

void EigenState::setScaling(double aScaling) {
	scaling = aScaling;
}

double EigenState::getScaling() const {
	return scaling;
}

size_t EigenState::size() const {
	return 1;
}

double EigenState::defineNormInf() const {
    double absMax = (double)probVec->vector.lpNorm<Eigen::Infinity>();
    return absMax;
}

void EigenState::addMult(double factor, const EigenState &otherState ) {
	assert(otherState.size() == size());
	probVec->vector += factor*otherState.probVec->vector;
}

void EigenState::initMult(double factor, const EigenState &otherState ) {
	resize();
	probVec->vector = factor*otherState.probVec->vector;
	scaling = otherState.scaling;
}

void EigenState::odeIntRelativeError(double m_eps_abs, double m_eps_rel, double m_a_x, double m_a_dxdt, const EigenState &aState1, const EigenState &aState2) {
	assert(aState1.size() == size() && aState2.size() == size());

	Eigen::VectorXd denom = (m_eps_abs + (m_eps_rel * ( m_a_x * aState1.probVec->vector.cwiseAbs() + m_a_dxdt * aState2.probVec->vector.cwiseAbs())).array()).matrix(); // @suppress("Invalid arguments") // @suppress("Method cannot be resolved")
	probVec->vector = probVec->vector.cwiseAbs().cwiseQuotient(denom);
}

EigenState& EigenState::operator+=( const double &val ) {
	//std::cout << " ADD val = " << val << " to id = " << id << std::endl;
	probVec->vector = (probVec->vector.array() + val).matrix();

	return *this;
}

EigenState& EigenState::operator+=( const EigenState &otherState ) {
	assert(otherState.size() == size());
	probVec->vector += otherState.probVec->vector;
	return *this;
}

EigenState& EigenState::operator*=( const double a ) {
	probVec->vector *= a;
	return *this;
}


EigenState& EigenState::operator=(const EigenState &otherState) {
	if(otherState.probVec != NULL) {
		resize();
		probVec->vector = otherState.probVec->vector;
		scaling = otherState.scaling;
	}

	return *this;
}

void EigenState::initMemory() {
	assert(probVec == NULL);
	probVec = Utils::MemoryPool::eigenCPU().threadSafeAllocateVector();
}


void EigenState::roundNegativeProbabilityToZero() {

	if((probVec->vector.array() < 0.).any()) {
		for(size_t iS=0; iS<(size_t)probVec->vector.size(); ++iS) {
			if(probVec->vector(iS) < 0.) {
				probVec->vector(iS) = 0.;
			}
		}
	}

}

void EigenState::rescaleProbabilities() {
	scaling += rescaleProbabilityVector(probVec->vector);
}


void EigenState::copyScalingFactors(const EigenState &otherState) {
	scaling = otherState.scaling;
}

EigenState operator/( const EigenState &otherState1 , const EigenState &otherState2 ) {
	assert(otherState1.size() == otherState2.size());
	EigenState newState(otherState1);

	newState.probVec->vector = otherState1.probVec->vector.cwiseQuotient(otherState2.probVec->vector);

	return newState;
}

EigenState abs( const EigenState &otherState ) {
	EigenState newState(otherState);

	newState.probVec->vector = otherState.probVec->vector.cwiseAbs();

	return newState;
}

} /* namespace Vector */
} /* namespace StateType */
} /* namespace Likelihood */
