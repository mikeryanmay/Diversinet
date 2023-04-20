/*
 * SimpleNetworkModel.h
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#ifndef SRC_MODELS_SIMPLENETWORKMODEL_H_
#define SRC_MODELS_SIMPLENETWORKMODEL_H_

#include "Models/BaseModel.h"
#include "Parameters/IncFwdParameterContainer.h"

namespace Models {

class SimpleNetworkModel: public BaseModel {

	public:

		SimpleNetworkModel(Parameters::ContainerSharedPtr aPtrParameter, size_t aKMax);
		virtual ~SimpleNetworkModel();

		const SpMat& getTransitionRateMatrix(double t);
		const SpMat& getSpeciationEventMatrix(double t);
		const SpMat& getDirectionalTriangleEventMatrix(double t);
		const SpMat& getBidirectionalTriangleEventMatrix(double t);
		const SpMat& getNewHybridTriangleEventMatrix(double t);
		const SpMat& getHybridDiamondEventMatrix(double t);
		const SpMat& getPolyploidTriangleEventMatrix(double t);
		const SpMat& getNewPolyploidTriangleEventMatrix(double t);
		const SpMat& getPolyploidDiamondEventMatrix(double t);

	private:

		void updateRateMatrix();
		void updateSpeciationEventMatrix();
		void updateDirectionalTriangleEventMatrix();
		void updateBidirectionalTriangleEventMatrix();
		void updateNewHybridTriangleEventMatrix();
		void updateHybridDiamondEventMatrix();
		void updatePolyploidTriangleEventMatrix();
		void updateNewPolyploidTriangleEventMatrix();
		void updatePolyploidDiamondEventMatrix();

};

} /* namespace Model */

#endif /* SRC_MODELS_SIMPLENETWORKMODEL_H_ */
