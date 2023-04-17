/*
 * Factory.h
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#ifndef SRC_MODELS_FACTORY_H_
#define SRC_MODELS_FACTORY_H_

#include <boost/smart_ptr/shared_ptr.hpp>

#include "Models/IncFwdModel.h"
#include "Parameters/IncFwdParameterContainer.h"

namespace Models {

class Factory {

	public:
		Factory();
		~Factory();

	static ModelSharedPtr createTimeHomogeneousNetworkModel(Parameters::ContainerSharedPtr ptrParameters, size_t kMax = 1024);

};

} /* namespace Model */

#endif /* SRC_MODELS_FACTORY_H_ */
