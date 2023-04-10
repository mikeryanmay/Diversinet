#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <boost/program_options.hpp>

#include <DiversinetInterface.h>

using namespace boost::program_options;

int main(int argc, const char* argv[]) {

	// containers for arguments
	double lambda, mu, eta, zeta, nu, psi, rho; // parameters
	int seed;
	double time; // simulation time
	bool extant; // whether to return extant trees
	std::string condition; // condition on survival or tree?
	size_t reps; // number of replicates
	std::string outfile; // the path to the output file

	// parse command-line arguments
	try {

		// declare the supported options.
		options_description desc("Options");
		desc.add_options()
		    ("help,h",                                                                    "print help message")
			("out,o",       value<std::string>(&outfile)->required(),                     "output file")
			("lambda,l",    value<double>(&lambda)->required(),                           "speciation rate")
			("mu,m",        value<double>(&mu)->required(),                               "extinction rate")
			("eta,e",       value<double>(&eta)->required(),                              "symmetrical hybridization rate")
			("zeta,z",      value<double>(&zeta)->required(),                             "asymmetrical hybridization rate")
			("nu,n",        value<double>(&nu)->required(),                               "hybrid speciation rate")
			("psi,s",       value<double>(&psi)->required(),                              "allopolyploid speciation rate")
			("rho,p",       value<double>(&rho)->required(),                              "sampling fraction at the present")
			("time,t",      value<double>(&time)->required(),                             "duration of the simulation")
			("extant,x",    value<bool>(&extant)->default_value(true),                    "prune extinct tips? (true or false)")
			("condition,c", value<std::string>(&condition)->default_value("tree+hybrid"), "condition of simulations (options: none, survival, tree, tree+hybrid)")
			("reps,r",      value<size_t>(&reps)->required(),                             "number of simulation replicates")
			("seed",        value<int>(&seed)->default_value(-1),                         "random number seed (optional)")
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
		if (vm.count("psi")) {
			std::cout << "Using allopolyploid speciation rate " << psi << "\n";
		}
		if (vm.count("rho")) {
			std::cout << "Using sampling fraction " << rho << "\n";
		}
		if (vm.count("time")) {
			std::cout << "Simulating for " << time << " time units.\n";
		}
		if (vm.count("condition")) {
			std::cout << "Conditioning on: " << condition << ".\n";
		}
		if (vm.count("extant")) {
			if ( extant == true ) {
				std::cout << "Including only extant species: true.\n";
			} else {
				std::cout << "Including only extant species: false.\n";
			}
		}
		if (vm.count("reps")) {
			std::cout << "Simulating " << reps << " replicate(s).\n";
		}
		if (vm.count("seed")) {
			if (seed == -1) {
				std::cout << "Using random number seed from clock.\n";
			} else {
				std::cout << "Using random number seed " << seed << "\n";
			}
		}
		if (vm.count("out")) {
			std::cout << "Writing output to file " << outfile << "\n";
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

	// simulate networks
	std::vector<std::string> sims = interface.simulate(time, condition, reps, seed, extant);

	// write to file
	std::ofstream output_file(outfile);
	std::ostream_iterator<std::string> output_iterator(output_file, "\n");
	std::copy(sims.begin(), sims.end(), output_iterator);

	// exit
	return 0;

}
