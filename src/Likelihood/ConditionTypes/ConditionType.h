/*
 * ConditionType.h
 *
 *  Created on: Sep 11, 2019
 *      Author: mrmay
 */

#ifndef LIKELIHOOD_CONDITIONTYPES_CONDITIONTYPE_H_
#define LIKELIHOOD_CONDITIONTYPES_CONDITIONTYPE_H_

#include <boost/assign/list_of.hpp>
#include <vector>
#include <cstddef>

namespace Likelihood {
namespace Conditions {

typedef enum {
	TIME = 0,
	ROOT_SURVIVAL = 1,
	ROOT_MRCA = 2,
	STEM_SURVIVAL = 3,
	STEM_TWO_SAMPLES = 4
} conditionalProbability_t;

const std::vector<std::string> CONDITION_NAMES = boost::assign::list_of("CONDITION_TIME")("CONDITION_ROOT_SURVIVAL")("CONDITION_ROOT_MRCA")("CONDITION_ROOT_SAMPLING")("CONDITION_STEM_SURVIVAL")("CONDITION_STEM_ONE_SAMPLE")("CONDITION_STEM_TWO_EXTANT")("CONDITION_STEM_TWO_SAMPLES")("CONDITION_ROOT_SAMPLING_AND_MRCA");

conditionalProbability_t intToConditionalProbabilityType(int myInt);

}
}

#endif /* LIKELIHOOD_CONDITIONTYPES_CONDITIONTYPE_H_ */
