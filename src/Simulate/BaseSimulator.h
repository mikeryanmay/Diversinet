/*
 * BaseSimulator.h
 *
 *  Created on: Apr 3, 2023
 *      Author: mike
 */

#ifndef SRC_SIMULATE_BASESIMULATOR_H_
#define SRC_SIMULATE_BASESIMULATOR_H_

#include <boost/random.hpp>

#include "../Data/Structure/IncFwdNetworkStructure.h"
#include "../Parameters/IncFwdParameterContainer.h"

typedef boost::mt19937 RNGType;

namespace Simulate {

class BaseSimulator {

	public:

		BaseSimulator(Parameters::ContainerSharedPtr someParams, int seed);
		~BaseSimulator();

		std::vector<Data::Structure::NetworkSharedPtr> simulate(double time, std::string condition, size_t nreps, bool extantOnly, int max_lineages);
		Data::Structure::NetworkSharedPtr              simulateNetwork(double time, int max_lineages);

	private:

		// internal simulation function
		bool simulateNetworkInternal(std::vector<Data::Structure::NodeSharedPtr>& activeNodes, std::vector<Data::Structure::NodeSharedPtr>& inactiveNodes, std::vector<Data::Structure::EdgeSharedPtr>& edges, double time, int max_lineages);

		// the RNG
		RNGType rng;

		// the parameters
		Parameters::ContainerSharedPtr ptrParams;


};

} /* namespace Simulate */

#endif /* SRC_SIMULATE_BASESIMULATOR_H_ */
