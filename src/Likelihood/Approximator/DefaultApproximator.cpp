/*
 * DenseApproximator.cpp
 *
 *  Created on: Apr 13, 2023
 *      Author: mike
 */

#include "DefaultApproximator.h"

#include "Likelihood/Kernels/CPU/IncEigenKernels.h"
#include "Likelihood/Scheduler/IncScheduler.h"
#include "Likelihood/Integrators/IntegratorFactory.h"
#include "Likelihood/Integrators/IntegratorFactory.hpp"
#include "Likelihood/Integrators/AdaptiveIntegrators.hpp"
#include "Likelihood/Scheduler/IncScheduler.h"
#include "Models/IncModel.h"

namespace Likelihood {
namespace Approximator {

DefaultApproximator::DefaultApproximator(Likelihood::Integrator::integrationScheme_t aIntScheme,
                                         Scheduler::SchedulerSharedPtr aPtrScheduler,
                                         Models::ModelSharedPtr aPtrModel,
										 Conditions::conditionalProbability_t aConditionType) :
	BaseApproximator(aIntScheme, aPtrScheduler, aPtrModel, aConditionType),
	N_MAX_STATE_VECTOR(1), /* only 1 for now (a single column vector) */
	kernels(aPtrModel),
	intKernel(N_MAX_STATE_VECTOR, aPtrModel) {

	// initialize the vector from the memory pool
	Utils::MemoryPool::eigenCPU().setNCategories(ptrModel->getNumberOfStates());
	Utils::MemoryPool::eigenCPU().setMaxStatesVector(N_MAX_STATE_VECTOR);

	// make the integrator
	ptrIntegrator = Likelihood::Integrator::Factory::createIntegrator<stateType_t, intKernel_t, operations_t>(DEFAULT_ABS_TOLERANCE, DEFAULT_REL_TOLERANCE, deltaT, aIntScheme);

}

DefaultApproximator::~DefaultApproximator() {
	assert(ptrIntegrator);
	delete ptrIntegrator;
}

void DefaultApproximator::setDefaultDeltaT(double aDeltaT) {
	assert(ptrIntegrator != nullptr);
	deltaT = aDeltaT;
	ptrIntegrator->setDeltaT(deltaT);
}

size_t DefaultApproximator::getTotalNumberOfIntegrationSteps() const {
	assert(ptrIntegrator != nullptr);
	return ptrIntegrator->getNSteps();
	return 0;
}


double DefaultApproximator::approximateLogLikelihood() {

	// By now we don't care if it has been updated
	ptrScheduler->clearHasBeenUpdatedFlag();

	logLikelihood = -std::numeric_limits<double>::max();

	// Enable specialized implementation operations: e.g., init state vectors
	doPreProcessingSteps();

	// Check the event validity and return 0 if an event is impossible
	if(!areEventsPossible()) {
		return logLikelihood;
	}

	size_t iEvent = 0;
	// Loop over integration/event steps until we reach the final event
	while(!ptrScheduler->getEvents()[iEvent]->checkEvent(Likelihood::Scheduler::FINAL_NODE_EVENT)) {
		// Do event related computations
		doEventStep(iEvent);
		// Integrate over the edges
		doIntegrationStep(iEvent);
		// Next event
		iEvent++;
	}

	// Final event compute the likelihood
	doEventStep(iEvent);

	// Enable specialized implementation operations: e.g., cleanup
	doPostProcessingSteps();

	// We should be good
	return logLikelihood;

}

void DefaultApproximator::doPreProcessingSteps() {
	probState.clear();
	integrationTimes.clear();
	ptrIntegrator->resetEvaluationState();
	scalingFactor = 0.0;
}

void DefaultApproximator::doIntegrationStep(size_t iEdgesLayer) {

	// iEdgesLayer starts at iEvent = iEdgesLayer and ends at iEvent+1=iEdgesLayer+1
	double startTime = ptrScheduler->getEvents()[iEdgesLayer]->getTime();
	double endTime = ptrScheduler->getEvents()[iEdgesLayer+1]->getTime();

	// tell the kernel the number of edges in this layer
	size_t numEdges = ptrScheduler->getNumEdgesForLayer(iEdgesLayer);
	ptrModel->setNumberOfLineages(numEdges);

	if(endTime == startTime) return;

	if(endTime != ptrScheduler->getEvents().back()->getTime()) {
		endTime = std::nextafter(endTime,-std::numeric_limits<double>::infinity());
	}

	// std::cout << startTime << " -- " << endTime << " -- " << numEdges << std::endl;

	assert(ptrIntegrator != NULL);
	ptrIntegrator->integrate(startTime, endTime, probState, intKernel);

	integrationTimes.insert(integrationTimes.end(), ptrIntegrator->getVecTimes().begin(), ptrIntegrator->getVecTimes().end()); // @suppress("Invalid arguments")

}

void DefaultApproximator::doEventStep(size_t iEvent) {

	// get the event
	Likelihood::Scheduler::Event* event = ptrScheduler->getEvents()[iEvent];

	// do the event
	if ( event->checkEvent(Likelihood::Scheduler::PRESENT_TIME_EVENT) ) {
		// std::cout << "present time event at time " << event->getTime() << std::endl;
		kernels.setInitialCondition(event->getNodes(), probState);
		kernels.rescaleProbabilities(probState);
	} else if ( event->checkEvent(Likelihood::Scheduler::SPECIATION_EVENT) ) {
		// std::cout << "speciation event at time " << event->getTime() << std::endl;
		kernels.computeSpeciationEvent(event->getTime(), probState);
		kernels.rescaleProbabilities(probState);
	} else if ( event->checkEvent(Likelihood::Scheduler::DIRECTIONAL_TRIANGLE) ) {
		// std::cout << "triangle event at time " << event->getTime() << std::endl;
		kernels.computeDirectionalTriangle(event->getTime(), probState);
		kernels.rescaleProbabilities(probState);
	} else if ( event->checkEvent(Likelihood::Scheduler::BIDIRECTIONAL_TRIANGLE) ) {
		// std::cout << "triangle event at time " << event->getTime() << std::endl;
		kernels.computeBidirectionalTriangle(event->getTime(), probState);
		kernels.rescaleProbabilities(probState);
	} else if ( event->checkEvent(Likelihood::Scheduler::NEW_HYBRID_TRIANGLE) ) {
		// std::cout << "hybrid triangle event at time " << event->getTime() << std::endl;
		kernels.computeNewHybridTriangle(event->getTime(), probState);
		kernels.rescaleProbabilities(probState);
	} else if ( event->checkEvent(Likelihood::Scheduler::HYBRID_DIAMOND) ) {
		// std::cout << "hybrid diamond event at time " << event->getTime() << std::endl;
		kernels.computeHybridDiamond(event->getTime(), probState);
		kernels.rescaleProbabilities(probState);
	} else if ( event->checkEvent(Likelihood::Scheduler::POLYPLOID_DIAMOND) ) {
		// std::cout << "polyploid diamond event at time " << event->getTime() << std::endl;
		kernels.computePolyploidDiamond(event->getTime(), probState);
		kernels.rescaleProbabilities(probState);
	} else if ( event->checkEvent(Likelihood::Scheduler::POLYPLOID_TRIANGLE) ) {
		// std::cout << "polyploid triangle event at time " << event->getTime() << std::endl;
		kernels.computePolyploidTriangle(event->getTime(), probState);
		kernels.rescaleProbabilities(probState);
	} else if ( event->checkEvent(Likelihood::Scheduler::NEW_POLYPLOID_TRIANGLE) ) {
		// std::cout << "new polyploid triangle event at time " << event->getTime() << std::endl;
		kernels.computeNewPolyploidTriangle(event->getTime(), probState);
		kernels.rescaleProbabilities(probState);
	} else if ( event->checkEvent(Likelihood::Scheduler::FINAL_NODE_EVENT) ) {
		// std::cout << "final node event at time " << event->getTime() << std::endl;
		logLikelihood = kernels.computeLogLikelihood(event->getTime(), probState);
		kernels.rescaleProbabilities(probState);
	} else if ( event->checkEvent(Likelihood::Scheduler::RESCALING_EVENT) ) {
		// std::cout << "rescaling event at time " << event->getTime() << std::endl;
		kernels.rescaleProbabilities(probState);
	} else {
		assert(false && "Event is not implemented.");
	}

}

void DefaultApproximator::doPostProcessingSteps() {
	
	// include the scaling factors in the log likelihood
	// logLikelihood += scalingFactor;
	logLikelihood += probState.getScaling();

}

void DefaultApproximator::doReportState(double t) {
}


} /* namespace Approximator */
} /* namespace Likelihood */
