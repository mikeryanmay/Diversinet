#include <NetDivInterface.h>
#include <iostream>
#include <vector>

#include "../Data/Structure/IncFwdNetworkStructure.h"
#include "../Data/Structure/IncNetworkStructure.h"
#include "../Simulate/BaseSimulator.h"
#include "../Parameters/IncParameterContainer.h"

namespace NetDiv {
namespace Interface {

NetDivInterface::NetDivInterface() : ptrParams(new Parameters::Container()) {
}

NetDivInterface::~NetDivInterface() {
}

// parameter setters
void NetDivInterface::setLambda(double lambda_) {
	ptrParams->lambda = lambda_;
}

void NetDivInterface::setMu(double mu_) {
	ptrParams->mu = mu_;
}

void NetDivInterface::setEta(double eta_) {
	ptrParams->eta = eta_;
}

void NetDivInterface::setZeta(double zeta_) {
	ptrParams->zeta = zeta_;
}

void NetDivInterface::setNu(double nu_) {
	ptrParams->nu = nu_;
}

void NetDivInterface::setRho(double rho_) {
	ptrParams->rho = rho_;
}

std::vector<std::string> NetDivInterface::simulate(double time, std::string condition, size_t nreps, int seed, bool extantOnly) {

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

} // end namespace interface
} // end namespace NETDIV
