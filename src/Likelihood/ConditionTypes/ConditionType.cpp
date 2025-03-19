/*
 * ConditionType.cpp
 *
 *  Created on: Sep 11, 2019
 *      Author: xmeyer
 */

#include "ConditionType.h"
#include <assert.h>
#include <iostream>

namespace Likelihood {
namespace Conditions {

conditionalProbability_t intToConditionalProbabilityType(int myInt) {
	switch (myInt) {
		case 0:
			return TIME;
			break;
		case 1:
			return ROOT_SURVIVAL;
			break;
		case 2:
			return ROOT_MRCA;
			break;
		case 3:
			return STEM_SURVIVAL;
			break;
		case 4:
			return STEM_TWO_SAMPLES;
			break;
		default:
			assert(false && "Unknown conditionalProbabilityType -- unsupported yet.");
			return TIME;
			break;
	}
}


}
}
