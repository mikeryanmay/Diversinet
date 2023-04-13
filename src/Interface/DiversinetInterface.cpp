#include <DiversinetInterface.h>
#include <iostream>
#include <vector>

#include <boost/make_shared.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include "../Data/Structure/IncFwdNetworkStructure.h"
#include "../Data/Structure/IncNetworkStructure.h"
#include "../Data/Reader/IncPhyloReader.h"
#include "../Parameters/IncParameterContainer.h"
#include "../Simulate/BaseSimulator.h"
#include "../Likelihood/Scheduler/BaseScheduler.h"

namespace Diversinet {
namespace Interface {

DiversinetInterface::DiversinetInterface() : ptrParams(new Parameters::Container()) {
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

double DiversinetInterface::computeLogLikelihood() {

	// only works if we have a network set
	assert(ptrNetwork && "No network set, cannot compute likelihood.");

	// update the parameters
	// TODO

	// initialize the scheduler
	if ( ptrScheduler == nullptr || schedulerOperation == RESET) {
		ptrScheduler.reset( new Likelihood::Scheduler::BaseScheduler(ptrNetwork) );
	} else if (schedulerOperation == UPDATE) {
		// TODO
	}

	double lnL = 0.0;

	return lnL;

}


std::vector<std::string> DiversinetInterface::simulate(double time, std::string condition, size_t nreps, int seed, bool extantOnly) {

	// check time argument
	bool check_time = time > 0.0;
	assert(check_time && "Invalid time provided. Ensure that time > 0.");

	// check condition
	bool check_condition = condition == "none" || condition == "survival" || condition == "tree" || condition == "tree+hybrid";
	assert(check_condition && "Invalid condition provided. Options are: none, survival, tree, tree+hybrid.");

	// create the simulator
	Simulate::BaseSimulator simulator(ptrParams, seed);

	// simulate the networks
	std::vector<Data::Structure::NetworkSharedPtr> networks = simulator.simulate(time, condition, nreps, extantOnly);

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




} // end namespace interface
} // end namespace diversinet
