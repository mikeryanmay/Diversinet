#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "boost/filesystem/fstream.hpp"

#include <DiversinetInterface.h>

using namespace boost::program_options;

int main(int argc, const char* argv[]) {

	// containers for arguments
	double lambda, mu, eta, zeta, nu, rho; // parameters
	std::string treefile; // the path to the output file

	// parse command-line arguments
	try {

		// declare the supported options.
		options_description desc("Options");
		desc.add_options()
		    ("help,h",                                                                    "print help message")
			("tree,t",      value<std::string>(&treefile)->required(),                    "tree file")
			("lambda,l",    value<double>(&lambda)->required(),                           "speciation rate")
			("mu,m",        value<double>(&mu)->required(),                               "extinction rate")
			("eta,e",       value<double>(&eta)->required(),                              "symmetrical hybridization rate")
			("zeta,z",      value<double>(&zeta)->required(),                             "asymmetrical hybridization rate")
			("nu,n",        value<double>(&nu)->required(),                               "hybrid speciation rate")
			("rho,p",       value<double>(&rho)->required(),                              "sampling fraction at the present")
		;

		// get the variable map
		variables_map vm;
		store(boost::program_options::parse_command_line(argc, argv, desc), vm);

		// check for help message
		if (vm.count("help")) {
		    std::cout << desc << "\n";
		}

		// notify the variable map (print help)
		notify(vm);

		// check other arguments
		if (vm.count("tree")) {
			std::cout << "Using tree file " << treefile << "\n";
		}
		if (vm.count("lambda")) {
			std::cout << "Using speciation rate " << lambda << "\n";
		}
		if (vm.count("mu")) {
			std::cout << "Using extinction rate " << mu << "\n";
		}
		if (vm.count("eta")) {
			std::cout << "Using asymmetrical hybridization rate " << eta << "\n";
		}
		if (vm.count("zeta")) {
			std::cout << "Using symmetrical hybridization rate " << zeta << "\n";
		}
		if (vm.count("nu")) {
			std::cout << "Using hybrid speciation rate " << nu << "\n";
		}
		if (vm.count("rho")) {
			std::cout << "Using sampling fraction " << rho << "\n";
		}

	} catch (const error &ex) {
		std::cerr << ex.what() << "\n";
		return 1;
	}

	// make the interface
	Diversinet::Interface::DiversinetInterface interface;

	// set the parameters
	interface.setLambda(lambda);
	interface.setMu(mu);
	interface.setEta(eta);
	interface.setZeta(zeta);
	interface.setNu(nu);
	interface.setRho(rho);

	// read the tree file
	std::ifstream file(treefile);
	std::string newick_string;
	std::getline(file, newick_string);

	// read the tree
	interface.readNewick(newick_string);
	interface.computeLogLikelihood();


//	// simulate networks
//	std::vector<std::string> sims = interface.simulate(time, condition, reps, seed, extant);
//
//	// write to file
//	std::ofstream output_file(outfile);
//	std::ostream_iterator<std::string> output_iterator(output_file, "\n");
//	std::copy(sims.begin(), sims.end(), output_iterator);

	// exit
	return 0;

}
