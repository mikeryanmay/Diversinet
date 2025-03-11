/*
 * Network.h
 *
 *  Created on: Apr 2, 2023
 *      Author: mike
 */

#ifndef SRC_DATA_STRUCTURE_NETWORK_H_
#define SRC_DATA_STRUCTURE_NETWORK_H_

#include <vector>
#include <set>

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

#include "IncFwdNetworkStructure.h"
#include "../Reader/IncFwdPhyloReader.h"

namespace Data {
namespace Structure {

class Network {

	public:

		Network(std::vector<NodeSharedPtr> someNodes, std::vector<EdgeSharedPtr> someEdges);
		Network(Data::NewickReader::NewickParserSharedPtr aNewickParser);

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

		void pruneExtinctTips();

		void jitterNetwork(double factor);

	private:

		// nodes and edges
		// only network gets shared pointers to these guys
		NodeSharedPtr oldestNode;
		std::vector<NodeSharedPtr> nodes;
		std::vector<EdgeSharedPtr> edges;

		// tips
		void updateNodes();
		std::vector<NodeSharedPtr> hybridNodes;
		std::vector<NodeSharedPtr> sampledTips;
		std::vector<NodeSharedPtr> extinctTips;
		size_t numHybridNodes   = 0;
		size_t numSampledTips   = 0;
		size_t numExtinctTips   = 0;

		void pruneExtinctTipsRecursive(NodeSharedPtr aNode);

		void buildNetworkFromNewick(const Data::NewickReader::TreeNode* aNewickRoot);
		double createRecursiveNewick(const NewickReader::TreeNode *newickNode, NodeSharedPtr treeNode);
		void mergeHybridNodesByLabels(std::string aLabel);

		void ensureSimultaneousEvents(double precision = 1.0e-6);
		void ensureSimultaneousEventsRecursive(NodeSharedPtr aNode, double precision);

		void jitterNetworkRecursive(NodeSharedPtr node, std::set<double>& ages, double factor);

		// RNG for jittering
		boost::mt19937 rng;

}; // end network


} // end structure namespace
} // end network namespace

#endif /* SRC_DATA_STRUCTURE_NETWORK_H_ */
