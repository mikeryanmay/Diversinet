/*
 * Edge.h
 *
 *  Created on: Apr 2, 2023
 *      Author: mike
 */

#ifndef SRC_DATA_STRUCTURE_EDGE_H_
#define SRC_DATA_STRUCTURE_EDGE_H_

#include <boost/smart_ptr/shared_ptr.hpp>
#include "IncFwdNetworkStructure.h"

namespace Data {
namespace Structure {

class Edge {

	public:

		Edge(NodeSharedPtr aParent, NodeSharedPtr aChild);
		~Edge();

		NodeSharedPtr getParent();
		void          setParent(NodeSharedPtr aParent);

		NodeSharedPtr getChild();
		void          setChild(NodeSharedPtr aChild);

		EdgeType getType() const;
		void setType(EdgeType aType);

		double getLength() const;

	protected:

		NodeSharedPtr parent, child;
		EdgeType type = Lineage;

	private:

		friend class Node;

}; // end edge

} /* namespace Structure */
} /* namespace Network */

#endif /* SRC_DATA_STRUCTURE_EDGE_H_ */
