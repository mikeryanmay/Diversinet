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

		Parameters::ContainerSharedPtr ptrParameters;

	private:

		size_t Kmax;

};

} /* namespace Model */

#endif /* SRC_MODELS_BASEMODEL_H_ */
