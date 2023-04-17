/*
 * Container.cpp
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#include <Models/BaseModel.h>
#include "Parameters/IncParameterContainer.h"

namespace Models {

BaseModel::BaseModel(Parameters::ContainerSharedPtr aPtrParameter, size_t aKMax) :
		Kmax(aKMax), ptrParameters(aPtrParameter) {
}

BaseModel::~BaseModel() {
}

size_t BaseModel::getNumberOfStates() const {
	return Kmax;
}

void BaseModel::setNumberOfStates(size_t aKmax) {
	Kmax = aKmax;
}

} /* namespace Model */
