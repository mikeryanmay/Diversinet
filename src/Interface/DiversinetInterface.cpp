#include <DiversinetInterface.h>
#include <iostream>
#include <vector>

#include <boost/make_shared.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include "../Data/Structure/IncFwdNetworkStructure.h"
#include "../Data/Structure/IncNetworkStructure.h"
#include "../Simulate/BaseSimulator.h"
#include "../Parameters/IncParameterContainer.h"
#include "../Data/Reader/IncPhyloReader.h"

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

void DiversinetInterface::setRho(double rho_) {
	ptrParams->rho = rho_;
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
	size_t i = 0;
	for(std::vector<Data::Structure::NetworkSharedPtr>::iterator it = networks.begin(); it != networks.end(); ++it) {
		std::cout << ++i << std::endl;
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

	// TEST: write it to a file
	std::string newickString = network.getNewickString();
    std::ofstream out("/Users/mike/repos/phyloploid_lib/files/testfile.txt");
    out << newickString;
    out << '\n';
    out.close();

//	std::vector<std::string> newickStrings;
//	newickStrings.push_back();
//
//	std::ofstream output_file("/Users/mike/repos/phyloploid_lib/files/testfile.txt");
//	std::ostream_iterator<std::string> output_iterator(output_file, "\n");
//	std::copy(newickStrings.begin(), newickStrings.end(), output_iterator);


	std::cout << "STOP RIGHT THERE" << std::endl;

}




} // end namespace interface
} // end namespace diversinet
