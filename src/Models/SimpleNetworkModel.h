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

	private:

};

} /* namespace Model */

#endif /* SRC_MODELS_SIMPLENETWORKMODEL_H_ */
