/*
 * SimpleNetworkModel.cpp
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#include "SimpleNetworkModel.h"
#include "Parameters/IncParameterContainer.h"

namespace Models {

SimpleNetworkModel::SimpleNetworkModel(Parameters::ContainerSharedPtr aPtrParameter, size_t aKMax = 1024) :
  BaseModel(aPtrParameter, aKMax) {
}

SimpleNetworkModel::~SimpleNetworkModel() {
}

} /* namespace Models */
