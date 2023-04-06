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

		double lambda = 1.0;
		double mu = 0.0;
		double eta = 0.0;
		double zeta = 0.0;
		double nu = 0.0;
		double rho = 1.0;

};

} /* namespace Parameters */

#endif /* SRC_PARAMETERS_CONTAINER_H_ */
