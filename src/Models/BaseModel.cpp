/*
 * Container.cpp
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#include <Eigen/Sparse>


#include "BaseModel.h"
#include "Parameters/IncParameterContainer.h"

namespace Models {

BaseModel::BaseModel(Parameters::ContainerSharedPtr aPtrParameter, size_t aKMax) :
		ptrParameters(aPtrParameter), Kmax(aKMax),
		transitionRateMatrix(Kmax, Kmax),
		speciationEventMatrix(Kmax, Kmax),
		directionalTriangleEventMatrix(Kmax, Kmax),
		bidirectionalTriangleEventMatrix(Kmax, Kmax),
		newHybridTriangleEventMatrix(Kmax, Kmax),
		hybridDiamondEventMatrix(Kmax, Kmax),
		polyploidTriangleEventMatrix(Kmax, Kmax),
		newPolyploidTriangleEventMatrix(Kmax, Kmax),
		polyploidDiamondEventMatrix(Kmax, Kmax) {
}

BaseModel::~BaseModel() {
}

size_t BaseModel::getNumberOfStates() const {
	return Kmax;
}

void BaseModel::setNeedsUpdateAll() {
	needsUpdateInitialProbabilities = true;
	needsUpdateRateMatrix = true;
	needsUpdateSpeciationEventMatrix = true;
	needsUpdateDirectionalTriangleEventMatrix = true;
	needsUpdateBidirectionalTriangleEventMatrix = true;
	needsUpdateNewHybridTriangleEventMatrix = true;
	needsUpdateHybridDiamondEventMatrix = true;
	needsUpdatePolyploidTriangleEventMatrix = true;
	needsUpdateNewPolyploidTriangleEventMatrix = true;
	needsUpdatePolyploidDiamondEventMatrix = true;
}

void BaseModel::setNeedsUpdate() {
	setNeedsUpdateAll();
}

void BaseModel::setNumberOfStates(size_t aKmax) {
	Kmax = aKmax;
	setNeedsUpdateAll();
}

void BaseModel::setNumberOfLineages(size_t aNumLineages) {
	numLineages = aNumLineages;
	needsUpdateRateMatrix = true;
}


} /* namespace Model */
