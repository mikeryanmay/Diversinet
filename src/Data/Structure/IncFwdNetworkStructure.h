/*
 * IncFwdNetworkStructure.h
 *
 *  Created on: Apr 2, 2023
 *      Author: mike
 */

#ifndef SRC_DATA_STRUCTURE_INCFWDNETWORKSTRUCTURE_H_
#define SRC_DATA_STRUCTURE_INCFWDNETWORKSTRUCTURE_H_

#include <boost/smart_ptr/weak_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace Data {
namespace Structure {

	class Edge;
	class Node;
	class Network;

	typedef boost::shared_ptr<Edge>    EdgeSharedPtr;
	typedef boost::weak_ptr<Edge>      EdgeWeakPtr;

	typedef boost::shared_ptr<Node>    NodeSharedPtr;
	typedef boost::weak_ptr<Node>      NodeWeakPtr;

	typedef boost::shared_ptr<Network> NetworkSharedPtr;
	typedef boost::weak_ptr<Network>   NetworkWeakPtr;

	enum NodeType {
		Origin, Root, Sample, Speciation, Extinction, Donor, Hybrid, HybridSpeciation
	};

	enum EdgeType {
		Lineage, Hybridization
	};

} /* namespace Structure */
} /* namespace Network */

namespace NS = ::Data::Structure;

#endif /* SRC_DATA_STRUCTURE_INCFWDNETWORKSTRUCTURE_H_ */
