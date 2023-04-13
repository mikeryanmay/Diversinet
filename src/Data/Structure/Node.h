/*
 * Node.h
 *
 *  Created on: Apr 2, 2023
 *      Author: mike
 */

#ifndef SRC_DATA_STRUCTURE_NODE_H_
#define SRC_DATA_STRUCTURE_NODE_H_

#include <vector>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "IncFwdNetworkStructure.h"

namespace Data {
namespace Structure {

class Node {

	public:

		Node(size_t aId, double aAge, NodeType aType);
		~Node();

		// type stuff
		NodeType getType() const;
		void setType(NodeType aType);
		bool validateType() const;

		// id stuff
		size_t getId() const;

		// label stuff
		std::string getLabel() const;
		void setLabel(std::string aLabel);

		// age stuff
		void setAge(double aAge);
		double getAge() const;

		// edge stuff
		std::vector<EdgeSharedPtr> getEdges() const;
		std::vector<EdgeSharedPtr> getEdgesToParents() const;
		std::vector<EdgeSharedPtr> getEdgesToChildren() const;
		std::vector<EdgeSharedPtr> getNonHorizontalEdgesToParents() const;

		// node stuff
		std::vector<NodeSharedPtr> getParentNodes() const;
		std::vector<NodeSharedPtr> getChildNodes() const;

		bool hasChild(NodeSharedPtr aNode) const;

		void addEdge(EdgeSharedPtr aEdge);
		void removeEdge(EdgeSharedPtr aEdge);

		void resetVisits();
		std::string recursivelyConstructNewickString(EdgeSharedPtr incomingEdge);

		size_t visits = 0;

	protected:

		size_t id;
		double age;
		NodeType type;

		// edges
		std::vector<EdgeWeakPtr> edges;

	private:

		friend class Edge;
		std::string label = "";

}; // end node

} /* namespace Structure */
} /* namespace Network */

#endif /* SRC_DATA_STRUCTURE_NODE_H_ */
