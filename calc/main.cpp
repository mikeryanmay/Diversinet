#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "boost/filesystem/fstream.hpp"

#include "DiversinetInterface.h"

using namespace boost::program_options;

int main(int argc, const char* argv[]) {

	// containers for arguments
	double lambda, mu, eta, zeta, nu, psi, rho; // parameters
	size_t kmax;
	std::string treefile; // the path to the output file
	bool screen;

	// parse command-line arguments
	try {

		// declare the supported options.
		options_description desc("Options");
		desc.add_options()
		    ("help,h",                                                                    "print help message")
			("tree,t",      value<std::string>(&treefile)->required(),                    "tree file")
			("kmax,k",      value<size_t>(&kmax)->required(),                             "the maximum number of states")
			("lambda,l",    value<double>(&lambda)->required(),                           "speciation rate")
			("mu,m",        value<double>(&mu)->required(),                               "extinction rate")
			("eta,e",       value<double>(&eta)->required(),                              "symmetrical hybridization rate")
			("zeta,z",      value<double>(&zeta)->required(),                             "asymmetrical hybridization rate")
			("nu,n",        value<double>(&nu)->required(),                               "hybrid speciation rate")
			("psi,s",       value<double>(&psi)->required(),                              "allopolyploid speciation rate")
			("rho,p",       value<double>(&rho)->required(),                              "sampling fraction at the present")
			("screen,o",    value<bool>(&screen)->default_value(true),                    "print parameters to screen")
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
		if (vm.count("tree") && screen) {
			std::cout << "Using tree file " << treefile << "\n";
		}
		if (vm.count("kmax") && screen) {
			std::cout << "Using maximum number of states " << kmax << "\n";
		}
		if (vm.count("lambda") && screen) {
			std::cout << "Using speciation rate " << lambda << "\n";
		}
		if (vm.count("mu") && screen) {
			std::cout << "Using extinction rate " << mu << "\n";
		}
		if (vm.count("eta") && screen) {
			std::cout << "Using asymmetrical hybridization rate " << eta << "\n";
		}
		if (vm.count("zeta") && screen) {
			std::cout << "Using symmetrical hybridization rate " << zeta << "\n";
		}
		if (vm.count("nu") && screen) {
			std::cout << "Using hybrid speciation rate " << nu << "\n";
		}
		if (vm.count("psi") && screen) {
			std::cout << "Using allopolyploid speciation rate " << psi << "\n";
		}
		if (vm.count("rho") && screen) {
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
	interface.setPsi(psi);
	interface.setRho(rho);
	interface.setKMax(kmax);

	// read the tree file
	std::ifstream file(treefile);
	std::string newick_string;
	std::getline(file, newick_string);

	// read the tree
	interface.readNewick(newick_string);
	double lnl = interface.computeLogLikelihood();

	// report to screen
	std::cout << "log likelihood is: " << std::setprecision(10) << lnl << std::endl;

	// exit
	return 0;

}
