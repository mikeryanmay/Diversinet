/*
 * Network.h
 *
 *  Created on: Apr 2, 2023
 *      Author: mike
 */

#ifndef SRC_DATA_STRUCTURE_NETWORK_H_
#define SRC_DATA_STRUCTURE_NETWORK_H_

#include <vector>
#include "IncFwdNetworkStructure.h"

namespace Data {
namespace Structure {

class Network {

	public:

		Network();
		Network(std::vector<NodeSharedPtr> someNodes, std::vector<EdgeSharedPtr> someEdges);
		~Network();

		const std::vector<NodeSharedPtr>& getNodes() const;
		const std::vector<EdgeSharedPtr>& getEdges() const;

		const NodeSharedPtr& getOldestNode() const;

		const std::vector<NodeSharedPtr>& getSampledTips() const;
		const std::vector<NodeSharedPtr>& getExtinctTips() const;
		const size_t& getNumSampledTips() const;
		const size_t& getNumExtinctTips() const;
		const size_t& getNumHybridNodes() const;

		void updateOldestNode();

		std::string getNewickString();

	private:

		// nodes and edges
		NodeSharedPtr oldestNode;
		std::vector<NodeSharedPtr> nodes;
		std::vector<EdgeSharedPtr> edges;

		// tips
		void updateNodes();
		std::vector<NodeSharedPtr> hybridNodes;
		std::vector<NodeSharedPtr> sampledTips;
		std::vector<NodeSharedPtr> extinctTips;
		size_t numHybridNodes = 0;
		size_t numSampledTips = 0;
		size_t numExtinctTips = 0;

}; // end network


} // end structure namespace
} // end network namespace

#endif /* SRC_DATA_STRUCTURE_NETWORK_H_ */
