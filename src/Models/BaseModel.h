/*
 * Container.h
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#ifndef SRC_MODELS_BASEMODEL_H_
#define SRC_MODELS_BASEMODEL_H_

#include <boost/smart_ptr/shared_ptr.hpp>

#include "IncFwdModel.h"
#include "Parameters/IncFwdParameterContainer.h"

namespace Models {

class BaseModel {

	public:

		BaseModel(Parameters::ContainerSharedPtr aPtrParameter, size_t aKMax);
		virtual ~BaseModel() = 0;

		size_t getNumberOfStates() const;
		void setNumberOfStates(size_t aKmax);
		void setNumberOfLineages(size_t aNumLineages);
		void setNeedsUpdate();

		// get initial probs
		virtual Eigen::VectorXd getInitialProbabilities(size_t numLineages) = 0;

		// get transition rates/event matrices
		virtual const SpMat& getTransitionRateMatrix(double t) = 0;
		virtual const SpMat& getSpeciationEventMatrix(double t) = 0;
		virtual const SpMat& getDirectionalTriangleEventMatrix(double t) = 0;
		virtual const SpMat& getBidirectionalTriangleEventMatrix(double t) = 0;
		virtual const SpMat& getNewHybridTriangleEventMatrix(double t) = 0;
		virtual const SpMat& getHybridDiamondEventMatrix(double t) = 0;
		virtual const SpMat& getPolyploidTriangleEventMatrix(double t) = 0;
		virtual const SpMat& getNewPolyploidTriangleEventMatrix(double t) = 0;
		virtual const SpMat& getPolyploidDiamondEventMatrix(double t) = 0;

	protected:

		// keep the parameters
		Parameters::ContainerSharedPtr ptrParameters;

		// update everything
		void setNeedsUpdateAll();

		// constants
		size_t Kmax;
		size_t numLineages = 0;

		// the rate matrix
		bool needsUpdateRateMatrix = true;
		SpMat transitionRateMatrix;
		virtual void updateRateMatrix() = 0;

		// speciation event matrix
		bool needsUpdateSpeciationEventMatrix = true;
		SpMat speciationEventMatrix;
		virtual void updateSpeciationEventMatrix() = 0;

		// directional triangle
		bool needsUpdateDirectionalTriangleEventMatrix = true;
		SpMat directionalTriangleEventMatrix;
		virtual void updateDirectionalTriangleEventMatrix() = 0;

		// bidirectional triangle
		bool needsUpdateBidirectionalTriangleEventMatrix = true;
		SpMat bidirectionalTriangleEventMatrix;
		virtual void updateBidirectionalTriangleEventMatrix() = 0;

		// hybrid species
		bool needsUpdateNewHybridTriangleEventMatrix = true;
		SpMat newHybridTriangleEventMatrix;
		virtual void updateNewHybridTriangleEventMatrix() = 0;

		// hybrid diamond
		bool needsUpdateHybridDiamondEventMatrix = true;
		SpMat hybridDiamondEventMatrix;
		virtual void updateHybridDiamondEventMatrix() = 0;

		// polyploid triangle
		bool needsUpdatePolyploidTriangleEventMatrix = true;
		SpMat polyploidTriangleEventMatrix;
		virtual void updatePolyploidTriangleEventMatrix() = 0;

		// polyploid species
		bool needsUpdateNewPolyploidTriangleEventMatrix = true;
		SpMat newPolyploidTriangleEventMatrix;
		virtual void updateNewPolyploidTriangleEventMatrix() = 0;

		// polyploid diamond
		bool needsUpdatePolyploidDiamondEventMatrix = true;
		SpMat polyploidDiamondEventMatrix;
		virtual void updatePolyploidDiamondEventMatrix() = 0;

	private:


};

} /* namespace Model */

#endif /* SRC_MODELS_BASEMODEL_H_ */
