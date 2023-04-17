/*
 * IncFwdModel.h
 *
 *  Created on: Apr 14, 2023
 *      Author: mike
 */

#ifndef SRC_MODELS_INCFWDMODEL_H_
#define SRC_MODELS_INCFWDMODEL_H_

#include <vector>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace Models {

	class Factory;
	class BaseModel;
	typedef boost::shared_ptr<BaseModel> ModelSharedPtr;

}




#endif /* SRC_MODELS_INCFWDMODEL_H_ */
