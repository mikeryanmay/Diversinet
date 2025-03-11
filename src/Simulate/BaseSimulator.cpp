/*
 * BaseSimulator.cpp
 *
 *  Created on: Apr 3, 2023
 *      Author: mike
 */

#include <vector>
#include <string>
#include <limits>

// #include <boost/progress.hpp>

#include <boost/make_shared.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

#include "BaseSimulator.h"
#include "../Parameters/IncParameterContainer.h"
#include "../Data/Structure/IncNetworkStructure.h"

namespace Simulate {

BaseSimulator::BaseSimulator(Parameters::ContainerSharedPtr someParams, int seed) :
	ptrParams(someParams) {

	if (seed == -1) {
		// use boost random device
		boost::random::random_device dev;
		rng.seed(dev);
	} else {
		// use user-specified value
		rng.seed(seed);
	}

}

BaseSimulator::~BaseSimulator() {
}

std::vector<Data::Structure::NetworkSharedPtr> BaseSimulator::simulate(double time, std::string condition, size_t nreps, bool extantOnly, int max_lineages, bool root) {

	// create the container for networks
	std::vector<Data::Structure::NetworkSharedPtr> networks;

	// create the trees
	Data::Structure::NetworkSharedPtr network;
	bool success;

	// std::cout << "\nSimulation in progress." << std::endl;
	// boost::progress_display show_progress( nreps );
	for(size_t i = 0; i < nreps; ++i) {

		// simulate for condition
		success = false;
		while ( success == false ) {

			// simulate a tree
			network = this->simulateNetwork(time, max_lineages, root);

			// if extant only, drop extinct tips
			if ( extantOnly ) {
				network->pruneExtinctTips();
			}

			// check conditions
			if ( condition == "none" ) {
				success = true;
			} else if ( condition == "survival" ) {
				success = network->getNumSampledTips() > 0;
			} else if ( condition == "tree" ) {
				success = network->getNumSampledTips() > 1;
			} else if ( condition == "tree+hybrid" ) {
				success = network->getNumSampledTips() > 1 && network->getNumHybridNodes() > 0;
			}

		}

		// add the simulation to the list
		networks.push_back(network);

		// increment progress bar
		// ++show_progress;

	}

	return networks;

}

Data::Structure::NetworkSharedPtr BaseSimulator::simulateNetwork(double time, int max_lineages, bool root) {

	using namespace Data::Structure;

	// interpret any max lineages < 1 as infinity lineages
	if (max_lineages < 1) {
		max_lineages = std::numeric_limits<int>::infinity();
	}

	// create containers for active and inactive nodes
	std::vector<NodeSharedPtr> activeNodes;
	std::vector<NodeSharedPtr> inactiveNodes;
	std::vector<EdgeSharedPtr> edges;

	while (true) {
		
		// try to simulate the network
		bool hit_max = simulateNetworkInternal(activeNodes, inactiveNodes, edges, time, max_lineages, root);

		if (hit_max) {

			// if we hit the max, clear and restart
			activeNodes.clear();
			inactiveNodes.clear();
			edges.clear();
	
		} else {

			// otherwise, terminate
			break;

		}

	}

	// create the network
	NS::Network network(inactiveNodes, edges);
	NetworkSharedPtr ptrNetwork = boost::make_shared<NS::Network>( network );

	// return
	return ptrNetwork;

}

bool BaseSimulator::simulateNetworkInternal(std::vector<Data::Structure::NodeSharedPtr>& activeNodes, std::vector<Data::Structure::NodeSharedPtr>& inactiveNodes, std::vector<Data::Structure::EdgeSharedPtr>& edges, double time, int max_lineages, bool root) {

	using namespace Data::Structure;

	// get the parameters
	double lambda = ptrParams->lambda;
	double mu     = ptrParams->mu;
	double eta    = ptrParams->eta;
	double zeta   = ptrParams->zeta;
	double nu     = ptrParams->nu;
	double psi    = ptrParams->psi;
	double rho    = ptrParams->rho;

	// create the uniform distribution
	boost::random::uniform_real_distribution<double> unif(0.0, 1.0);
	boost::variate_generator<RNGType&, boost::random::uniform_real_distribution<> > runif(rng, unif);

	// create the initial nodes and edge
	size_t id = 0;
	NodeSharedPtr originNode = boost::make_shared<Node>( Node(id++, time, Origin) );
	inactiveNodes.push_back(originNode);

	NodeSharedPtr rootNode = boost::make_shared<Node>( Node(id++, time, Root) );

	EdgeSharedPtr stem = boost::make_shared<Edge>( Edge(originNode, rootNode) );
	edges.push_back(stem);

	originNode->addEdge(stem);
	rootNode->addEdge(stem);

	if (!root) {
		
		// simulating starting from origin
		// root begins as activeNode
		activeNodes.push_back(rootNode);

	} else {

		// simulate from root
		// root begins as inactiveNode
		inactiveNodes.push_back(rootNode);

		// make two new nodes, add them to activeNodes
		NodeSharedPtr leftNode = boost::make_shared<Node>( Node(id++, time, Speciation) );
		NodeSharedPtr rightNode = boost::make_shared<Node>( Node(id++, time, Speciation) );
		
		EdgeSharedPtr leftEdge = boost::make_shared<Edge>( Edge(rootNode, leftNode) );
		rootNode->addEdge(leftEdge);
		leftNode->addEdge(leftEdge);
		edges.push_back(leftEdge);

		EdgeSharedPtr rightEdge = boost::make_shared<Edge>( Edge(rootNode, rightNode) );
		rootNode->addEdge(rightEdge);
		rightNode->addEdge(rightEdge);
		edges.push_back(rightEdge);

		activeNodes.push_back(leftNode);
		activeNodes.push_back(rightNode);

	}

	// simulate
	double num_active, num_choose_two;
	double speciation_rate, extinction_rate, asym_hybridization_rate, sym_hybridization_rate, hybrid_speciation_rate, allopolyploid_rate;
	double total_rate;
	double current_time = time, waiting_time;
	double u;
	size_t num_spec = 0, num_ext = 0, num_lgt = 0, num_hyb = 0, num_allo = 0;

	// simulate until time is done
	while (true) {

		// if no active lineages, terminate
		if ( activeNodes.size() == 0 ) {
			break;
		}

		// if we hit the maximum number of lineages, terminate
		if ( activeNodes.size() == max_lineages ) {
			return true;
		}

		// compute the number of active lineages
		num_active     = activeNodes.size();
		num_choose_two = num_active * (num_active - 1.0) * 2.0;

		// compute the rates of events
		speciation_rate         = num_active * lambda;
		extinction_rate         = num_active * mu;
		asym_hybridization_rate = num_choose_two * eta;
		sym_hybridization_rate  = num_choose_two * zeta;
		hybrid_speciation_rate  = num_choose_two * nu;
		allopolyploid_rate      = num_choose_two * psi;

		// compute the total rate
		total_rate = speciation_rate + extinction_rate + asym_hybridization_rate + sym_hybridization_rate + hybrid_speciation_rate + allopolyploid_rate;

		// draw a waiting time
		u = runif();
		waiting_time = -std::log(u) / total_rate;
		current_time -= waiting_time;

		// check if we exceed time of simulation
		if ( current_time < 0 ) {
			current_time = 0;
			break;
		}

		// otherwise, pick an event
		u = runif() * total_rate;

		// do a speciation event
		u -= speciation_rate;
		if ( u < 0.0 ) {

			++num_spec;

			// pick the affected lineage
			size_t affected_lineage = runif() * num_active;

			// get the affected lineage
			NodeSharedPtr parent = activeNodes.at(affected_lineage);

			// change the age and type
			parent->setAge(current_time);
			parent->setType(Speciation);

			// create new lineages
			NodeSharedPtr left  = boost::make_shared<Node>( Node(id++, current_time, Speciation) );
			NodeSharedPtr right = boost::make_shared<Node>( Node(id++, current_time, Speciation) );

			// create the new edges
			EdgeSharedPtr leftEdge  = boost::make_shared<Edge>( Edge(parent, left) );
			parent->addEdge(leftEdge);
			left->addEdge(leftEdge);

			EdgeSharedPtr rightEdge = boost::make_shared<Edge>( Edge(parent, right) );
			parent->addEdge(rightEdge);
			right->addEdge(rightEdge);

			// remove the parent from active nodes, move to inactive nodes
			activeNodes.erase(activeNodes.begin() + affected_lineage); // @suppress("Invalid arguments")
			inactiveNodes.push_back(parent);

			// add new daughters to active nodes
			activeNodes.push_back(left);
			activeNodes.push_back(right);

			// add edges to vector of edges
			edges.push_back(leftEdge);
			edges.push_back(rightEdge);

			// do next event
			continue;

		}

		// do an extinction event
		u -= extinction_rate;
		if ( u < 0.0 ) {

			++num_ext;

			// pick the affected lineage
			size_t affected_lineage = runif() * num_active;

			// get the affected lineage
			NodeSharedPtr extinct = activeNodes.at(affected_lineage);

			// change the age and type
			extinct->setAge(current_time);
			extinct->setType(Extinction);

			// give the tip a label
			extinct->setLabel("e_" + std::to_string(num_ext));

			// remove the extinct node from active nodes, move to inactive nodes
			activeNodes.erase(activeNodes.begin() + affected_lineage); // @suppress("Invalid arguments")
			inactiveNodes.push_back(extinct);

			// do next event
			continue;

		}

		// do an asymmetrical hybridization event
		u -= asym_hybridization_rate;
		if ( u < 0.0 ) {

			++num_lgt;

			// pick the affected lineages
			size_t left_lineage = runif() * num_active;
			size_t right_lineage = left_lineage;
			while (left_lineage == right_lineage) {
				right_lineage = runif() * num_active;
			}

			// get the affected lineages
			NodeSharedPtr leftParent  = activeNodes.at(left_lineage);
			NodeSharedPtr rightParent = activeNodes.at(right_lineage);

			// change the age and type
			leftParent->setAge(current_time);
			leftParent->setType(Donor);

			rightParent->setAge(current_time);
			rightParent->setType(Hybrid);
			rightParent->setLabel("#LGT" + std::to_string(num_lgt));

			// create new lineages
			NodeSharedPtr left  = boost::make_shared<Node>( Node(id++, current_time, Speciation) );
			NodeSharedPtr right = boost::make_shared<Node>( Node(id++, current_time, Speciation) );

			// create the new edges
			EdgeSharedPtr hybridEdge = boost::make_shared<Edge>( Edge(leftParent, rightParent) );
			hybridEdge->setType(Hybridization);
			leftParent->addEdge(hybridEdge);
			rightParent->addEdge(hybridEdge);

			EdgeSharedPtr leftEdge = boost::make_shared<Edge>( Edge(leftParent, left) );
			leftParent->addEdge(leftEdge);
			left->addEdge(leftEdge);

			EdgeSharedPtr rightEdge = boost::make_shared<Edge>( Edge(rightParent, right) );
			rightParent->addEdge(rightEdge);
			right->addEdge(rightEdge);

			// remove the parents from active nodes
			std::vector<NodeSharedPtr>::iterator it = activeNodes.begin();
			size_t nr = 0;
			while ( nr < 2 && it != activeNodes.end() ) {
				// check if index is for left
				if ( (*it)->getId() == leftParent->getId() || (*it)->getId() == rightParent->getId() ) {
					// if so, delete
					activeNodes.erase(it); // @suppress("Invalid arguments")
					nr++;
				} else {
					// otherwise, move to the next value
					it++;
				}
			}

			// add the parents to the inactive nodes
			inactiveNodes.push_back(leftParent);
			inactiveNodes.push_back(rightParent);

			// add new daughters to active nodes
			activeNodes.push_back(left);
			activeNodes.push_back(right);

			// add edges to vector of edges
			edges.push_back(hybridEdge);
			edges.push_back(leftEdge);
			edges.push_back(rightEdge);

			// do next event
			continue;

		}

		// do an symmetrical hybridization event
		u -= sym_hybridization_rate;
		if ( u < 0.0 ) {

			// pick the affected lineages
			size_t left_lineage = runif() * num_active;
			size_t right_lineage = left_lineage;
			while (left_lineage == right_lineage) {
				right_lineage = runif() * num_active;
			}

			// get the affected lineages
			NodeSharedPtr leftParent  = activeNodes.at(left_lineage);
			NodeSharedPtr rightParent = activeNodes.at(right_lineage);

			// change the age and type
			leftParent->setAge(current_time);
			leftParent->setType(Hybrid);
			leftParent->setLabel("#LGT" + std::to_string(++num_lgt));

			rightParent->setAge(current_time);
			rightParent->setType(Hybrid);
			rightParent->setLabel("#LGT" + std::to_string(++num_lgt));

			// create new lineages
			NodeSharedPtr left  = boost::make_shared<Node>( Node(id++, current_time, Speciation) );
			NodeSharedPtr right = boost::make_shared<Node>( Node(id++, current_time, Speciation) );

			// create the new edges
			EdgeSharedPtr leftHybridEdge = boost::make_shared<Edge>( Edge(leftParent, rightParent) );
			leftHybridEdge->setType(Hybridization);
			leftParent->addEdge(leftHybridEdge);
			rightParent->addEdge(leftHybridEdge);

			EdgeSharedPtr rightHybridEdge = boost::make_shared<Edge>( Edge(rightParent, leftParent) );
			rightHybridEdge->setType(Hybridization);
			rightParent->addEdge(rightHybridEdge);
			leftParent->addEdge(rightHybridEdge);

			EdgeSharedPtr leftEdge = boost::make_shared<Edge>( Edge(leftParent, left) );
			leftParent->addEdge(leftEdge);
			left->addEdge(leftEdge);

			EdgeSharedPtr rightEdge = boost::make_shared<Edge>( Edge(rightParent, right) );
			rightParent->addEdge(rightEdge);
			right->addEdge(rightEdge);

			// remove the parents from active nodes
			std::vector<NodeSharedPtr>::iterator it = activeNodes.begin();
			size_t nr = 0;
			while ( nr < 2 && it != activeNodes.end() ) {
				// check if index is for left
				if ( (*it)->getId() == leftParent->getId() || (*it)->getId() == rightParent->getId() ) {
					// if so, delete
					activeNodes.erase(it); // @suppress("Invalid arguments")
					nr++;
				} else {
					// otherwise, move to the next value
					it++;
				}
			}

			// add the parents to the inactive nodes
			inactiveNodes.push_back(leftParent);
			inactiveNodes.push_back(rightParent);

			// add new daughters to active nodes
			activeNodes.push_back(left);
			activeNodes.push_back(right);

			// add edges to vector of edges
			edges.push_back(leftHybridEdge);
			edges.push_back(rightHybridEdge);
			edges.push_back(leftEdge);
			edges.push_back(rightEdge);

			// do next event
			continue;

		}

		// do a hybrid speciation event
		u -= hybrid_speciation_rate;
		if ( u < 0.0 ) {

			++num_hyb;

			// pick the affected lineages
			size_t left_lineage = runif() * num_active;
			size_t right_lineage = left_lineage;
			while (left_lineage == right_lineage) {
				right_lineage = runif() * num_active;
			}

			// get the affected lineages
			NodeSharedPtr leftParent  = activeNodes.at(left_lineage);
			NodeSharedPtr rightParent = activeNodes.at(right_lineage);

			// change the age and type
			leftParent->setAge(current_time);
			leftParent->setType(Donor);

			rightParent->setAge(current_time);
			rightParent->setType(Donor);

			// create new lineages
			NodeSharedPtr left     = boost::make_shared<Node>( Node(id++, current_time, Speciation) );
			NodeSharedPtr middle   = boost::make_shared<Node>( Node(id++, current_time, HybridSpecies) );
			NodeSharedPtr daughter = boost::make_shared<Node>( Node(id++, current_time, Speciation) );
			NodeSharedPtr right    = boost::make_shared<Node>( Node(id++, current_time, Speciation) );

			// add label to the middle node
			middle->setLabel("#H" + std::to_string(num_hyb));

			// create the new edges
			EdgeSharedPtr leftToMiddle = boost::make_shared<Edge>( Edge(leftParent, middle) );
			leftToMiddle->setType(Hybridization);
			leftParent->addEdge(leftToMiddle);
			middle->addEdge(leftToMiddle);

			EdgeSharedPtr leftToLeft = boost::make_shared<Edge>( Edge(leftParent, left) );
			leftParent->addEdge(leftToLeft);
			left->addEdge(leftToLeft);

			EdgeSharedPtr rightToMiddle = boost::make_shared<Edge>( Edge(rightParent, middle) );
			rightToMiddle->setType(Hybridization);
			rightParent->addEdge(rightToMiddle);
			middle->addEdge(rightToMiddle);

			EdgeSharedPtr rightToRight = boost::make_shared<Edge>( Edge(rightParent, right) );
			rightParent->addEdge(rightToRight);
			right->addEdge(rightToRight);

			EdgeSharedPtr middleToDaughter = boost::make_shared<Edge>( Edge(middle, daughter) );
			middle->addEdge(middleToDaughter);
			daughter->addEdge(middleToDaughter);

			// remove the parents from active nodes
			std::vector<NodeSharedPtr>::iterator it = activeNodes.begin();
			size_t nr = 0;
			while ( nr < 2 && it != activeNodes.end() ) {
				// check if index is for left
				if ( (*it)->getId() == leftParent->getId() || (*it)->getId() == rightParent->getId() ) {
					// if so, delete
					activeNodes.erase(it); // @suppress("Invalid arguments")
					nr++;
				} else {
					// otherwise, move to the next value
					it++;
				}
			}

			// add the parents to the inactive nodes
			inactiveNodes.push_back(leftParent);
			inactiveNodes.push_back(rightParent);
			inactiveNodes.push_back(middle);

			// add new daughters to active nodes
			activeNodes.push_back(left);
			activeNodes.push_back(right);
			activeNodes.push_back(daughter);

			// add edges to vector of edges
			edges.push_back(leftToMiddle);
			edges.push_back(leftToLeft);
			edges.push_back(rightToMiddle);
			edges.push_back(rightToRight);
			edges.push_back(middleToDaughter);

			// do next event
			continue;

		}

		// do a hybrid speciation event
		u -= allopolyploid_rate;
		if ( u < 0.0 ) {

			++num_allo;

			// pick the affected lineages
			size_t left_lineage = runif() * num_active;
			size_t right_lineage = left_lineage;
			while (left_lineage == right_lineage) {
				right_lineage = runif() * num_active;
			}

			// get the affected lineages
			NodeSharedPtr leftParent  = activeNodes.at(left_lineage);
			NodeSharedPtr rightParent = activeNodes.at(right_lineage);

			// change the age and type
			leftParent->setAge(current_time);
			leftParent->setType(Donor);

			rightParent->setAge(current_time);
			rightParent->setType(Donor);

			// create new lineages
			NodeSharedPtr left     = boost::make_shared<Node>( Node(id++, current_time, Speciation) );
			NodeSharedPtr middle   = boost::make_shared<Node>( Node(id++, current_time, Allopolyploid) );
			NodeSharedPtr daughter = boost::make_shared<Node>( Node(id++, current_time, Speciation) );
			NodeSharedPtr right    = boost::make_shared<Node>( Node(id++, current_time, Speciation) );

			// add label to the middle node
			middle->setLabel("#P" + std::to_string(num_allo));

			// create the new edges
			EdgeSharedPtr leftToMiddle = boost::make_shared<Edge>( Edge(leftParent, middle) );
			leftToMiddle->setType(Hybridization);
			leftParent->addEdge(leftToMiddle);
			middle->addEdge(leftToMiddle);

			EdgeSharedPtr leftToLeft = boost::make_shared<Edge>( Edge(leftParent, left) );
			leftParent->addEdge(leftToLeft);
			left->addEdge(leftToLeft);

			EdgeSharedPtr rightToMiddle = boost::make_shared<Edge>( Edge(rightParent, middle) );
			rightToMiddle->setType(Hybridization);
			rightParent->addEdge(rightToMiddle);
			middle->addEdge(rightToMiddle);

			EdgeSharedPtr rightToRight = boost::make_shared<Edge>( Edge(rightParent, right) );
			rightParent->addEdge(rightToRight);
			right->addEdge(rightToRight);

			EdgeSharedPtr middleToDaughter = boost::make_shared<Edge>( Edge(middle, daughter) );
			middle->addEdge(middleToDaughter);
			daughter->addEdge(middleToDaughter);

			// remove the parents from active nodes
			std::vector<NodeSharedPtr>::iterator it = activeNodes.begin();
			size_t nr = 0;
			while ( nr < 2 && it != activeNodes.end() ) {
				// check if index is for left
				if ( (*it)->getId() == leftParent->getId() || (*it)->getId() == rightParent->getId() ) {
					// if so, delete
					activeNodes.erase(it); // @suppress("Invalid arguments")
					nr++;
				} else {
					// otherwise, move to the next value
					it++;
				}
			}

			// add the parents to the inactive nodes
			inactiveNodes.push_back(leftParent);
			inactiveNodes.push_back(rightParent);
			inactiveNodes.push_back(middle);

			// add new daughters to active nodes
			activeNodes.push_back(left);
			activeNodes.push_back(right);
			activeNodes.push_back(daughter);

			// add edges to vector of edges
			edges.push_back(leftToMiddle);
			edges.push_back(leftToLeft);
			edges.push_back(rightToMiddle);
			edges.push_back(rightToRight);
			edges.push_back(middleToDaughter);

			// do next event
			continue;

		}

		if ( u > 0 ) {
			std::cout << "OOPS" << std::endl;
		}

	}

	// sample each extant node with some probability
	size_t num_samples = 0;
	for(std::vector<NodeSharedPtr>::iterator it = activeNodes.begin(); it != activeNodes.end(); ++it) {

		// set the age to the present
		(*it)->setAge(0.0);

		// check whether it's sampled;
		u = runif();
		if ( u < rho ) {
			(*it)->setType(Sample);
			(*it)->setLabel("t_" + std::to_string(++num_samples));
		} else {
			(*it)->setType(Extinction);
			(*it)->setLabel("e_" + std::to_string(++num_ext));
		}

		// add to the inactive nodes list
		inactiveNodes.push_back( *it );

	}

	return false;

}

} /* namespace Simulate */
