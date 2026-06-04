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

		// get rate/event matrices
		const Eigen::VectorXd& getInitialProbabilities(size_t initialNumberOfLineages) override;
		const SpMat& getTransitionRateMatrix(double t) override;
		const SpMat& getSpeciationEventMatrix(double t) override;
		const SpMat& getDirectionalTriangleEventMatrix(double t) override;
		const SpMat& getBidirectionalTriangleEventMatrix(double t) override;
		const SpMat& getNewHybridTriangleEventMatrix(double t) override;
		const SpMat& getHybridDiamondEventMatrix(double t) override;
		const SpMat& getPolyploidTriangleEventMatrix(double t) override;
		const SpMat& getNewPolyploidTriangleEventMatrix(double t) override;
		const SpMat& getPolyploidDiamondEventMatrix(double t) override;

		// apply rate/event operators
		void computeTransitionRateAction(Eigen::VectorXd &dxdt, const Eigen::VectorXd &p, double t) override;
		void applySpeciationEvent(Eigen::VectorXd &p, double t) override;
		void applyDirectionalTriangleEvent(Eigen::VectorXd &p, double t) override;
		void applyBidirectionalTriangleEvent(Eigen::VectorXd &p, double t) override;
		void applyNewHybridTriangleEvent(Eigen::VectorXd &p, double t) override;
		void applyHybridDiamondEvent(Eigen::VectorXd &p, double t) override;
		void applyPolyploidTriangleEvent(Eigen::VectorXd &p, double t) override;
		void applyNewPolyploidTriangleEvent(Eigen::VectorXd &p, double t) override;
		void applyPolyploidDiamondEvent(Eigen::VectorXd &p, double t) override;

	private:

		void resizeIfKmaxChanged(SpMat &matrix);
		void resizeEventScratch(size_t size);
		void updateInitialProbabilities(size_t initialNumberOfLineages) override;
		void updateRateMatrix() override;
		void updateSpeciationEventMatrix() override;
		void updateDirectionalTriangleEventMatrix() override;
		void updateBidirectionalTriangleEventMatrix() override;
		void updateNewHybridTriangleEventMatrix() override;
		void updateHybridDiamondEventMatrix() override;
		void updatePolyploidTriangleEventMatrix() override;
		void updateNewPolyploidTriangleEventMatrix() override;
		void updatePolyploidDiamondEventMatrix() override;

		Eigen::VectorXd eventScratch;

	};

} /* namespace Model */

#endif /* SRC_MODELS_SIMPLENETWORKMODEL_H_ */
