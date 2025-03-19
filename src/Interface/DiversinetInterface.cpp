#include <DiversinetInterface.h>
#include <iostream>
#include <vector>

#include <boost/make_shared.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include "Data/Structure/IncFwdNetworkStructure.h"
#include "Data/Structure/IncNetworkStructure.h"
#include "Data/Reader/IncPhyloReader.h"
#include "Likelihood/Scheduler/BaseScheduler.h"
#include "Likelihood/ConditionTypes/ConditionType.h"
#include "Likelihood/Approximator/Factory.h"
#include "Parameters/IncParameterContainer.h"
#include "Models/IncModel.h"
#include "Simulate/BaseSimulator.h"


namespace Diversinet {
namespace Interface {

DiversinetInterface::DiversinetInterface() :
	ptrParams(new Parameters::Container()) {
}

DiversinetInterface::~DiversinetInterface() {
}

// parameter setters
void DiversinetInterface::setLambda(double lambda_) {
	ptrParams->lambda = lambda_;
}

void DiversinetInterface::setMu(double mu_) {
	ptrParams->mu = mu_;
}

void DiversinetInterface::setEta(double eta_) {
	ptrParams->eta = eta_;
}

void DiversinetInterface::setZeta(double zeta_) {
	ptrParams->zeta = zeta_;
}

void DiversinetInterface::setNu(double nu_) {
	ptrParams->nu = nu_;
}

void DiversinetInterface::setPsi(double psi_) {
	ptrParams->psi = psi_;
}

void DiversinetInterface::setRho(double rho_) {
	ptrParams->rho = rho_;
}

void DiversinetInterface::setKMax(size_t kmax_) {
	kMax = kmax_;
	if ( ptrModel != nullptr ) {
		ptrModel->setNumberOfStates(kMax);
		// schedulerOperation = RESET;
		// dirtyApproximator = true;
		// ptrModel = nullptr;
	}
}

void DiversinetInterface::setKMaxInt(int kmax_) {
	kMax = (size_t)kmax_;
	if ( ptrModel != nullptr ) {
		ptrModel->setNumberOfStates(kMax);
		// schedulerOperation = RESET;
		// dirtyApproximator = true;
		// ptrModel = nullptr;
	}
}

void DiversinetInterface::setConditionalProbabilityType(int aCondProb) {
	condProbType = static_cast<conditionalProbability_t>(aCondProb);
	dirtyApproximator = true;
}

double DiversinetInterface::computeLogLikelihood() {

	// only works if we have a network set
	assert(ptrNetwork && "No network set, cannot compute likelihood.");

	// initialize the model
	if ( ptrModel == nullptr ) {
		if (modelVersion == SIMPLE) {
			ptrModel = Models::Factory::createTimeHomogeneousNetworkModel(ptrParams, kMax);
		} else {
			assert(ptrModel && "Requested model is not available.");
		}
	}
	assert(ptrModel && "Model is invalid.");

	// make sure parameters are updated
	ptrModel->setNeedsUpdate();

	// initialize the scheduler
	if ( ptrScheduler == nullptr || schedulerOperation == RESET) {
		ptrScheduler.reset( new Likelihood::Scheduler::BaseScheduler(ptrNetwork) );
		ptrScheduler->defineAndSetRescalingEvents();
	} else if (schedulerOperation == UPDATE) {
	}
	assert(ptrScheduler && "Scheduler is invalid.");

	// get the condition type
	Likelihood::Conditions::conditionalProbability_t condType = Likelihood::Conditions::intToConditionalProbabilityType(condProbType);

	// initialize the approximator
	if ( ptrApproximator == nullptr || dirtyApproximator ) {
		Likelihood::Integrator::integrationScheme_t intType = Likelihood::Integrator::intToIntegratorType(integrationScheme);
		if ( approxVersion == DEFAULT ) {
			ptrApproximator = Likelihood::Approximator::Factory::createDefaultApproximator(intType, ptrScheduler, ptrModel, condType);
		} else {
			assert(ptrApproximator && "Requested approximator not available.");
		}
		dirtyApproximator = false;
	}
	assert(ptrApproximator && "Approximator is invalid.");

	// now other settings to the approximator
	ptrApproximator->setDefaultDeltaT(initDeltaT);

	// compute the log likelihood
	double logLik = ptrApproximator->approximateLogLikelihood();

	// clear the scheduler operation
	schedulerOperation = NONE;

	return logLik;

}

std::string DiversinetInterface::simulate(double time, std::string condition, int seed, bool extantOnly, int max_lineages, bool root) {

	// check time argument
	bool check_time = time > 0.0;
	assert(check_time && "Invalid time provided. Ensure that time > 0.");

	// check condition
	bool check_condition = condition == "none" || condition == "survival" || condition == "tree" || condition == "tree+hybrid";
	assert(check_condition && "Invalid condition provided. Options are: none, survival, tree, tree+hybrid.");

	// create the simulator
	Simulate::BaseSimulator simulator(ptrParams, seed);

	// simulate the network
	std::vector<Data::Structure::NetworkSharedPtr> networks = simulator.simulate(time, condition, 1, extantOnly, max_lineages, root);

	// translate to newick string
	std::string newick = networks.at(0)->getNewickString();

	return newick;

}

std::vector<std::string> DiversinetInterface::simulate(double time, std::string condition, size_t nreps, int seed, bool extantOnly, int max_lineages, bool root) {

	// check time argument
	bool check_time = time > 0.0;
	assert(check_time && "Invalid time provided. Ensure that time > 0.");

	// check condition
	bool check_condition = condition == "none" || condition == "survival" || condition == "tree" || condition == "tree+hybrid";
	assert(check_condition && "Invalid condition provided. Options are: none, survival, tree, tree+hybrid.");

	// create the simulator
	Simulate::BaseSimulator simulator(ptrParams, seed);

	// simulate the networks
	std::vector<Data::Structure::NetworkSharedPtr> networks = simulator.simulate(time, condition, nreps, extantOnly, max_lineages, root);

	// translate to newick strings
	std::vector<std::string> newicks;
	for(std::vector<Data::Structure::NetworkSharedPtr>::iterator it = networks.begin(); it != networks.end(); ++it) {
		newicks.push_back( (*it)->getNewickString() );
	}

	return newicks;

}

void DiversinetInterface::readNewick(std::string newick) {

	using namespace Data::NewickReader;

	// first, get the newick parsed object
	NewickParser parsedNewick(newick, NewickParser::IS_TREE_STRING);
	NewickParserSharedPtr ptrParsedNewick = boost::make_shared<NewickParser>(parsedNewick);

	// now pass it to the network constructor
	Data::Structure::Network network(ptrParsedNewick);

	// assign the pointer
	ptrNetwork = boost::make_shared<Data::Structure::Network>(network);

}

std::string DiversinetInterface::jitterNewick(std::string newick, double factor) {

	using namespace Data::NewickReader;

	// first, get the newick parsed object
	NewickParser parsedNewick(newick, NewickParser::IS_TREE_STRING);
	NewickParserSharedPtr ptrParsedNewick = boost::make_shared<NewickParser>(parsedNewick);

	// now pass it to the network constructor
	Data::Structure::Network network(ptrParsedNewick);

	// now jitter the network
	network.jitterNetwork(factor);

	// finally, get the newick string
	std::string jittered_newick = network.getNewickString();
	return jittered_newick;

}


} // end namespace interface
} // end namespace diversinet
