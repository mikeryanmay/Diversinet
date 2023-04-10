/*
 * Container.h
 *
 *  Created on: Apr 3, 2023
 *      Author: mike
 */

#ifndef SRC_PARAMETERS_CONTAINER_H_
#define SRC_PARAMETERS_CONTAINER_H_

namespace Parameters {

class Container {

	public:

		Container();
		~Container();

	public:

		double lambda = 1.0; // speciation rate
		double mu = 0.0;     // extinction rate
		double eta = 0.0;    // asymmetrical hybridization rate
		double zeta = 0.0;   // symmetrical hybridiziation rate
		double psi = 0.0;    // allopolyploidization rate
		double nu = 0.0;     // hybrid speciation rate
		double rho = 1.0;    // sampling fraction at the present

};

} /* namespace Parameters */

#endif /* SRC_PARAMETERS_CONTAINER_H_ */
