/*
 * IncFwdParameterContainer.h
 *
 *  Created on: Apr 3, 2023
 *      Author: mike
 */

#ifndef SRC_PARAMETERS_INCFWDPARAMETERCONTAINER_H_
#define SRC_PARAMETERS_INCFWDPARAMETERCONTAINER_H_

#include <boost/smart_ptr/shared_ptr.hpp>

namespace Parameters {
	class Container;
	typedef boost::shared_ptr<Container> ContainerSharedPtr;
}


#endif /* SRC_PARAMETERS_INCFWDPARAMETERCONTAINER_H_ */
