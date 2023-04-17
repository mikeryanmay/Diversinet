/*
 * Factory.cpp
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include "Factory.h"
#include "Parameters/Container.h"
#include "Models/IncModel.h"
#include "Models/SimpleNetworkModel.h"

namespace Models {

Factory::Factory() {

}

Factory::~Factory() {
}


ModelSharedPtr Factory::createTimeHomogeneousNetworkModel(Parameters::ContainerSharedPtr ptrParameters, size_t kMax) {
	// create the simple network model
	boost::shared_ptr<SimpleNetworkModel> ptrSimple = boost::make_shared<SimpleNetworkModel>(ptrParameters, kMax);
	ModelSharedPtr ptrModel = ptrSimple; // @suppress("Invalid arguments")
	return ptrModel;
}

} /* namespace Model */
