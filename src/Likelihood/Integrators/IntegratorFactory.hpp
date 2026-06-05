/*
 * IntegratorFactory.hpp
 *
 *  Created on: Dec 6, 2019
 *      Author: meyerx
 */

#include "IntegratorFactory.h"

#include "AdaptiveIntegrators.hpp"

#include <boost/numeric/odeint.hpp>

#ifndef LIKELIHOOD_CUSTOMINTEGRATORS_INTEGRATORFACTORY_DEF_H_
#define LIKELIHOOD_CUSTOMINTEGRATORS_INTEGRATORFACTORY_DEF_H_

namespace Likelihood {
namespace Integrator {

/************************************************/
/*************** BASE INTEGRATOR ****************/
/************************************************/

template <class StateType, class IntegratorKernel, class OperationType>
const size_t Base<StateType, IntegratorKernel, OperationType>::DELTA_T_ACCUMULATOR_WINDOW_SIZE = 100;

template <class StateType, class IntegratorKernel, class OperationType>
Base<StateType, IntegratorKernel, OperationType>::Base(const double aAbsError, const double aRelError, const double aDeltaT) :
												  DELTA_T(aDeltaT), ABS_ERROR(aAbsError), REL_ERROR(aRelError) {
	nSteps = 0;
}

template <class StateType, class IntegratorKernel, class OperationType>
Base<StateType, IntegratorKernel, OperationType>::~Base() {
}

template <class StateType, class IntegratorKernel, class OperationType>
void Base<StateType, IntegratorKernel, OperationType>::setDeltaT(double aDeltaT) {
	DELTA_T = aDeltaT;
}

template <class StateType, class IntegratorKernel, class OperationType>
void Base<StateType, IntegratorKernel, OperationType>::resetEvaluationState() {
	nSteps = 0;
	vecTimes.clear();
	reset();
}

template <class StateType, class IntegratorKernel, class OperationType>
size_t Base<StateType, IntegratorKernel, OperationType>::getNSteps() const {
	return nSteps;
}

template <class StateType, class IntegratorKernel, class OperationType>
const std::vector<double>& Base<StateType, IntegratorKernel, OperationType>::getVecTimes() const {
	return vecTimes;
}


/************************************************/
/*****************    EULER    ******************/
/************************************************/
template <class StateType, class IntegratorKernel, class OperationType>
Euler<StateType, IntegratorKernel, OperationType>::Euler(const double aAbsError, const double aRelError, const double aDeltaT) :
												   Base<StateType, IntegratorKernel, OperationType>(aAbsError, aRelError, aDeltaT) {
}

template <class StateType, class IntegratorKernel, class OperationType>
Euler<StateType, IntegratorKernel, OperationType>::~Euler() {
}

template <class StateType, class IntegratorKernel, class OperationType>
int Euler<StateType, IntegratorKernel, OperationType>::integrate(double startTime, double endTime,
																 StateType &state, IntegratorKernel &intKernel) {
	
	// We make sure that the start and end time are at least as far apart as machine precision
	if(std::fabs(endTime - startTime) < std::numeric_limits<double>::epsilon()) return 0;

	int steps = integrate_const( constStepper, boost::ref(intKernel), state, startTime, endTime, Base<StateType, IntegratorKernel, OperationType>::DELTA_T);
	state.roundNegativeProbabilityToZero();
	Base<StateType, IntegratorKernel, OperationType>::nSteps += steps;
	return steps;
}

template <class StateType, class IntegratorKernel, class OperationType>
void Euler<StateType, IntegratorKernel, OperationType>::reset() {
	Base<StateType, IntegratorKernel, OperationType>::vecTimes.clear();
}

/************************************************/
/*************    Runge Kutta 4    **************/
/************************************************/
template <class StateType, class IntegratorKernel, class OperationType>
RungeKutta4<StateType, IntegratorKernel, OperationType>::RungeKutta4(const double aAbsError, const double aRelError, const double aDeltaT) :
														 Base<StateType, IntegratorKernel, OperationType>(aAbsError, aRelError, aDeltaT) {
}

template <class StateType, class IntegratorKernel, class OperationType>
RungeKutta4<StateType, IntegratorKernel, OperationType>::~RungeKutta4() {
}

template <class StateType, class IntegratorKernel, class OperationType>
int RungeKutta4<StateType, IntegratorKernel, OperationType>::integrate(double startTime, double endTime,
																	   StateType &state, IntegratorKernel &intKernel) {
	
	// We make sure that the start and end time are at least as far apart as machine precision
	if(std::fabs(endTime - startTime) < std::numeric_limits<double>::epsilon()) return 0;

	int steps = integrate_const( constStepper, boost::ref(intKernel), state, startTime, endTime, Base<StateType, IntegratorKernel, OperationType>::DELTA_T);
	state.roundNegativeProbabilityToZero();
	Base<StateType, IntegratorKernel, OperationType>::nSteps += steps;
	return steps;
}

template <class StateType, class IntegratorKernel, class OperationType>
void RungeKutta4<StateType, IntegratorKernel, OperationType>::reset() {
	Base<StateType, IntegratorKernel, OperationType>::vecTimes.clear();
}

/************************************************/
/***************    Exponential    **************/
/************************************************/
template <class StateType, class IntegratorKernel, class OperationType>
Exponential<StateType, IntegratorKernel, OperationType>::Exponential(const double aAbsError, const double aRelError, const double aDeltaT) :
												   Base<StateType, IntegratorKernel, OperationType>(aAbsError, aRelError, aDeltaT) {
}

template <class StateType, class IntegratorKernel, class OperationType>
Exponential<StateType, IntegratorKernel, OperationType>::~Exponential() {
}

template <class StateType, class IntegratorKernel, class OperationType>
int Exponential<StateType, IntegratorKernel, OperationType>::integrate(double startTime, double endTime,
																 StateType &state, IntegratorKernel &intKernel) {
	
	if(std::fabs(endTime - startTime) < std::numeric_limits<double>::epsilon()) return 0;

	const double deltaT = endTime - startTime;
	const Eigen::MatrixXd rateMatrix = Eigen::MatrixXd(intKernel.getTransitionRateMatrix(startTime));
	state.getStateProb() = (rateMatrix * deltaT).exp() * state.getStateProb();
	state.roundNegativeProbabilityToZero();
	Base<StateType, IntegratorKernel, OperationType>::nSteps += 1;
	return 1;
}

template <class StateType, class IntegratorKernel, class OperationType>
void Exponential<StateType, IntegratorKernel, OperationType>::reset() {
	Base<StateType, IntegratorKernel, OperationType>::vecTimes.clear();
}

/************************************************/
/*************    Uniformization    *************/
/************************************************/
template <class StateType, class IntegratorKernel, class OperationType>
Uniformization<StateType, IntegratorKernel, OperationType>::Uniformization(const double aAbsError, const double aRelError, const double aDeltaT) :
												   Base<StateType, IntegratorKernel, OperationType>(aAbsError, aRelError, aDeltaT) {
}

template <class StateType, class IntegratorKernel, class OperationType>
Uniformization<StateType, IntegratorKernel, OperationType>::~Uniformization() {
}

template <class StateType, class IntegratorKernel, class OperationType>
int Uniformization<StateType, IntegratorKernel, OperationType>::integrate(double startTime, double endTime,
																 StateType &state, IntegratorKernel &intKernel) {
	
	if(std::fabs(endTime - startTime) < std::numeric_limits<double>::epsilon()) return 0;

	const double deltaT = endTime - startTime;
	const double omega = intKernel.getUniformizationRate(startTime);

	if(omega <= 0.0) {
		return 0;
	}

	const double theta = omega * deltaT;
	if(theta <= 0.0) {
		return 0;
	}

	const double poissonTolerance = std::min(1.0e-15, Base<StateType, IntegratorKernel, OperationType>::ABS_ERROR);
	const int mode = (int)std::floor(theta);
	const double sigma = std::sqrt(theta);
	int lower = std::max(0, (int)std::floor(theta - 10.0 * sigma - 10.0));
	int upper = std::max(lower, (int)std::ceil(theta + 10.0 * sigma + 10.0));

	std::vector<double> poissonWeights((size_t)(upper - lower + 1), 0.0);
	poissonWeights[(size_t)(mode - lower)] = 1.0;
	double weightSum = 1.0;
	double weight = 1.0;
	for(int n = mode; n > lower; --n) {
		weight *= (double)n / theta;
		poissonWeights[(size_t)(n - 1 - lower)] = weight;
		weightSum += weight;
	}
	weight = 1.0;
	for(int n = mode + 1; n <= upper; ++n) {
		weight *= theta / (double)n;
		poissonWeights[(size_t)(n - lower)] = weight;
		weightSum += weight;
	}

	while(lower > 0) {
		const double nextWeight = poissonWeights.front() * (double)lower / theta;
		const double nextRatio = (double)(lower - 1) / theta;
		if(nextRatio < 1.0 && nextWeight * nextRatio / (1.0 - nextRatio) <= poissonTolerance * weightSum) {
			break;
		}
		poissonWeights.insert(poissonWeights.begin(), nextWeight);
		weightSum += nextWeight;
		--lower;
	}

	while(true) {
		const double nextWeight = poissonWeights.back() * theta / (double)(upper + 1);
		const double nextRatio = theta / (double)(upper + 2);
		if(nextRatio < 1.0 && nextWeight * nextRatio / (1.0 - nextRatio) <= poissonTolerance * weightSum) {
			break;
		}
		poissonWeights.push_back(nextWeight);
		weightSum += nextWeight;
		++upper;
	}

	StateType term(state);
	StateType rateAction;
	Eigen::VectorXd result = Eigen::VectorXd::Zero(state.getStateProb().size());

	for(int n = 0; n <= upper; ++n) {
		if(n >= lower) {
			result += poissonWeights[(size_t)(n - lower)] * term.getStateProb();
		}

		if(n < upper) {
			intKernel(term, rateAction, startTime);
			term.getStateProb() += rateAction.getStateProb() / omega;
		}
	}

	state.getStateProb() = result / weightSum;
	state.roundNegativeProbabilityToZero();
	Base<StateType, IntegratorKernel, OperationType>::nSteps += (size_t)(upper + 1);
	return upper + 1;
}

template <class StateType, class IntegratorKernel, class OperationType>
void Uniformization<StateType, IntegratorKernel, OperationType>::reset() {
	Base<StateType, IntegratorKernel, OperationType>::vecTimes.clear();
}

/************************************************/
/************    Runge Kutta 45    **************/
/************************************************/
template <class StateType, class IntegratorKernel, class OperationType>
RungeKutta54<StateType, IntegratorKernel, OperationType>::RungeKutta54(const double aAbsError, const double aRelError, const double aDeltaT) :
														  Base<StateType, IntegratorKernel, OperationType>(aAbsError, aRelError, aDeltaT),
														  deltaTAcc(boost::accumulators::tag::rolling_window::window_size = Base<StateType, IntegratorKernel, OperationType>::DELTA_T_ACCUMULATOR_WINDOW_SIZE),
														  adaptiveStepper(make_controlled( Base<StateType, IntegratorKernel, OperationType>::ABS_ERROR,
																  	  	  	  	  	  	   Base<StateType, IntegratorKernel, OperationType>::REL_ERROR, rk54_stepper_t())) {
}

template <class StateType, class IntegratorKernel, class OperationType>
RungeKutta54<StateType, IntegratorKernel, OperationType>::~RungeKutta54() {
}

template <class StateType, class IntegratorKernel, class OperationType>
int RungeKutta54<StateType, IntegratorKernel, OperationType>::integrate(double startTime, double endTime,
																	    StateType &state, IntegratorKernel &intKernel) {

	
	// We make sure that the start and end time are at least as far apart as machine precision
	if(std::fabs(endTime - startTime) < std::numeric_limits<double>::epsilon()) return 0;

	Base<StateType, IntegratorKernel, OperationType>::vecTimes.clear();
	double deltaT = Base<StateType, IntegratorKernel, OperationType>::DELTA_T;
	if(boost::accumulators::rolling_count(deltaTAcc) > Base<StateType, IntegratorKernel, OperationType>::DELTA_T_ACCUMULATOR_WINDOW_SIZE/2) {
		deltaT =  boost::accumulators::rolling_mean(deltaTAcc);
	}

	std::vector<double> vecDeltaT;
	Base<StateType, IntegratorKernel, OperationType>::vecTimes.clear();
	int steps = integrate_adaptive_custom( boost::ref(adaptiveStepper), boost::ref(intKernel), state, startTime, endTime, deltaT, vecDeltaT, Base<StateType, IntegratorKernel, OperationType>::vecTimes);
	Base<StateType, IntegratorKernel, OperationType>::nSteps += steps;
	RungeKutta54<StateType, IntegratorKernel, OperationType>::reset();

	for(size_t iD=1; iD<vecDeltaT.size()-1; ++iD) {
		deltaTAcc(vecDeltaT[iD]);
	}

	return steps;
}

template <class StateType, class IntegratorKernel, class OperationType>
void RungeKutta54<StateType, IntegratorKernel, OperationType>::reset() {
	// hard reset
	/*adaptiveStepper = make_controlled(Base<StateType, IntegratorKernel, OperationType>::ABS_ERROR,
	  	  	  	   Base<StateType, IntegratorKernel, OperationType>::REL_ERROR, rk54_stepper_t());*/
	//adaptiveStepper.reset();
}

template <class StateType, class IntegratorKernel, class OperationType>
void RungeKutta54<StateType, IntegratorKernel, OperationType>::resetEvaluationState() {
	Base<StateType, IntegratorKernel, OperationType>::resetEvaluationState();
	deltaTAcc = delta_t_accumulator_t(boost::accumulators::tag::rolling_window::window_size = Base<StateType, IntegratorKernel, OperationType>::DELTA_T_ACCUMULATOR_WINDOW_SIZE);
}

/************************************************/
/*********    Runge Kutta DOPRI 5    ************/
/************************************************/
template <class StateType, class IntegratorKernel, class OperationType>
RungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::RungeKuttaDOPRI5(const double aAbsError, const double aRelError, const double aDeltaT) :
															  Base<StateType, IntegratorKernel, OperationType>(aAbsError, aRelError, aDeltaT),
															  deltaTAcc(boost::accumulators::tag::rolling_window::window_size = Base<StateType, IntegratorKernel, OperationType>::DELTA_T_ACCUMULATOR_WINDOW_SIZE),
															  adaptiveStepper(make_controlled(Base<StateType, IntegratorKernel, OperationType>::ABS_ERROR,
																	                          Base<StateType, IntegratorKernel, OperationType>::REL_ERROR, rkd5_stepper_t())) {
}

template <class StateType, class IntegratorKernel, class OperationType>
RungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::~RungeKuttaDOPRI5() {
}

template <class StateType, class IntegratorKernel, class OperationType>
int RungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::integrate(double startTime, double endTime,
																		    StateType &state, IntegratorKernel &intKernel) {

	
	// We make sure that the start and end time are at least as far apart as machine precision
	if(std::fabs(endTime - startTime) < std::numeric_limits<double>::epsilon()) return 0;

	Base<StateType, IntegratorKernel, OperationType>::vecTimes.clear();
	double deltaT = Base<StateType, IntegratorKernel, OperationType>::DELTA_T;
	if(boost::accumulators::rolling_count(deltaTAcc) > Base<StateType, IntegratorKernel, OperationType>::DELTA_T_ACCUMULATOR_WINDOW_SIZE/2) {
		deltaT = boost::accumulators::rolling_mean(deltaTAcc);
	}

	std::vector<double> vecDeltaT;
	Base<StateType, IntegratorKernel, OperationType>::vecTimes.clear();
	int steps = integrate_adaptive_custom( boost::ref(adaptiveStepper), boost::ref(intKernel), state, startTime, endTime , deltaT, vecDeltaT, Base<StateType, IntegratorKernel, OperationType>::vecTimes);
	reset();

	Base<StateType, IntegratorKernel, OperationType>::nSteps += steps;

	for(size_t iD=1; iD<vecDeltaT.size()-1; ++iD) {
		deltaTAcc(vecDeltaT[iD]);
	}
	return steps;
}

template <class StateType, class IntegratorKernel, class OperationType>
void RungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::reset() {
	// hard reset
	adaptiveStepper.reset();
	//adaptiveStepper = make_controlled(Base<StateType, IntegratorKernel, OperationType>::ABS_ERROR,
	//				  Base<StateType, IntegratorKernel, OperationType>::REL_ERROR, rkd5_stepper_t());
}

template <class StateType, class IntegratorKernel, class OperationType>
void RungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::resetEvaluationState() {
	Base<StateType, IntegratorKernel, OperationType>::resetEvaluationState();
	deltaTAcc = delta_t_accumulator_t(boost::accumulators::tag::rolling_window::window_size = Base<StateType, IntegratorKernel, OperationType>::DELTA_T_ACCUMULATOR_WINDOW_SIZE);
}

/************************************************/
/****** Dense Runge Kutta DOPRI 5    **********/
/************************************************/
template <class StateType, class IntegratorKernel, class OperationType>
DenseRungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::DenseRungeKuttaDOPRI5(const double aAbsError, const double aRelError, const double aDeltaT) :
															  Base<StateType, IntegratorKernel, OperationType>(aAbsError, aRelError, aDeltaT),
															  adaptiveStepper(make_controlled(Base<StateType, IntegratorKernel, OperationType>::ABS_ERROR,
																	                       Base<StateType, IntegratorKernel, OperationType>::REL_ERROR, rkd5_stepper_t())) {

}

template <class StateType, class IntegratorKernel, class OperationType>
DenseRungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::~DenseRungeKuttaDOPRI5() {
}

template <class StateType, class IntegratorKernel, class OperationType>
int DenseRungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::integrate(double startTime, double endTime,
																		    StateType &state, IntegratorKernel &intKernel) {

	// We make sure that the start and end time are at least as far apart as machine precision
	if(std::fabs(endTime - startTime) < std::numeric_limits<double>::epsilon()) return 0;

	Base<StateType, IntegratorKernel, OperationType>::vecTimes.clear();
	double deltaT = Base<StateType, IntegratorKernel, OperationType>::DELTA_T;

	std::vector<double> vecDeltaT;
	Base<StateType, IntegratorKernel, OperationType>::vecTimes.clear();
	boost::numeric::odeint::integrate_adaptive( boost::ref(adaptiveStepper), boost::ref(intKernel), state, startTime, endTime, deltaT);
	//reset();

	Base<StateType, IntegratorKernel, OperationType>::nSteps = adaptiveStepper.getSteppers().size();

	for(size_t iT=0; iT < adaptiveStepper.getStepTimes().size(); ++iT ) {
		Base<StateType, IntegratorKernel, OperationType>::vecTimes.push_back(adaptiveStepper.getStepTimes()[iT].second);
	}

	return Base<StateType, IntegratorKernel, OperationType>::vecTimes.size();
}

template <class StateType, class IntegratorKernel, class OperationType>
void DenseRungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::reset() {
	//adaptiveStepper.reset();
}

template <class StateType, class IntegratorKernel, class OperationType>
const std::vector< typename DenseRungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::rkd5_stepper_t >& DenseRungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::getSteppers() const {
	return adaptiveStepper.getSteppers();
}

template <class StateType, class IntegratorKernel, class OperationType>
const std::vector< std::pair<StateType, StateType> >& DenseRungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::getStepStates() const {
	return adaptiveStepper.getStepStates();
}

template <class StateType, class IntegratorKernel, class OperationType>
const std::vector< std::pair<StateType, StateType> >& DenseRungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::getStepDerivs() const {
	return adaptiveStepper.getStepDerivs();
}

template <class StateType, class IntegratorKernel, class OperationType>
const std::vector< std::pair<double, double> >& DenseRungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::getStepTimes() const {
	return adaptiveStepper.getStepTimes();
}

template <class StateType, class IntegratorKernel, class OperationType>
void DenseRungeKuttaDOPRI5<StateType, IntegratorKernel, OperationType>::transferStepsMemory(
						 std::vector< rkd5_stepper_t >& aSteppers,
						 std::vector< std::pair< double, double > >& aTimes,
						 std::vector< std::pair< StateType, StateType >  >& aStepStates,
						 std::vector< std::pair< StateType, StateType >  >& aStepDerivs ) {
	adaptiveStepper.transferStepsMemory(boost::ref(aSteppers), aTimes, aStepStates, aStepDerivs);
}

/************************************************/
/***************    FACTORY    ******************/
/************************************************/

} /* namespace Integrator */
} /* namespace Likelihood */

#endif /* LIKELIHOOD_CUSTOMINTEGRATORS_INTEGRATORFACTORY_DEF_H_ */
