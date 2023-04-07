#ifndef _NETDIV_INTERFACE_
#define _NETDIV_INTERFACE_

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

namespace NetDiv {
namespace Interface {

class NetDivInterface {

	public:

		NetDivInterface();
		~NetDivInterface();

		// parameter setters
		void setLambda(double lambda_);
		void setMu(double mu_);
		void setEta(double eta_);
		void setZeta(double zeta_);
		void setNu(double nu_);
		void setRho(double rho_);

		// simulate
		std::vector<std::string> simulate(double time, std::string condition, size_t nreps, int seed, bool extantOnly);

	private:

		// parameter values
		Parameters::ContainerSharedPtr ptrParams;

};

} // end namespace interface
} // end namespace phyloploid

#endif // _NETDIV_INTERFACE_
