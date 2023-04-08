#ifndef _DIVNET_INTERFACE_
#define _DIVNET_INTERFACE_

#include <vector>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace Data {
namespace Structure {
	class Network;
	typedef boost::shared_ptr<Network> NetworkSharedPtr;
}
}

namespace Parameters {
	class Container;
	typedef boost::shared_ptr<Container> ContainerSharedPtr;
}

namespace Diversinet {
namespace Interface {

class DiversinetInterface {

	public:

		DiversinetInterface();
		~DiversinetInterface();

		// parameter setters
		void setLambda(double lambda_);
		void setMu(double mu_);
		void setEta(double eta_);
		void setZeta(double zeta_);
		void setNu(double nu_);
		void setRho(double rho_);

		// simulate
		std::vector<std::string> simulate(double time, std::string condition, size_t nreps, int seed, bool extantOnly);

		// read a network
		void readNewick(std::string newick);

	private:

		// parameter values
		Parameters::ContainerSharedPtr ptrParams;

		// network variable
		Data::Structure::NetworkSharedPtr ptrNetwork;

};

} // end namespace interface
} // end namespace diversinet

#endif // _DIVNET_INTERFACE_
