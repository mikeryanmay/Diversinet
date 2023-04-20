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

namespace Models {
	class BaseModel;
	typedef boost::shared_ptr<BaseModel> ModelSharedPtr;
}

namespace Likelihood {

namespace Scheduler {
	class BaseScheduler;
	typedef boost::shared_ptr<BaseScheduler> SchedulerSharedPtr;
}

namespace Approximator {
	class BaseApproximator;
	typedef boost::shared_ptr<BaseApproximator> ApproximatorSharedPtr;
}

}

namespace Diversinet {
namespace Interface {

typedef enum {
	DEFAULT = 0
} approximatorVersion_t;

typedef enum {
	SIMPLE = 0
} modelVersion_t;

typedef enum {
	EULER = 0,
	RUNGE_KUTTA4 = 1,
	RUNGE_KUTTA54 = 2,
	RUNGE_KUTTA_DOPRI5 = 3
} integrationScheme_t;

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
		void setPsi(double psi_);
		void setRho(double rho_);
		void setKMax(size_t kmax_);

		// likelihood
		double computeLogLikelihood();

		// simulate
		std::vector<std::string> simulate(double time, std::string condition, size_t nreps, int seed, bool extantOnly);

		// read a network
		void readNewick(std::string newick);

	private:

		// parameter values
		Parameters::ContainerSharedPtr ptrParams;

		// model
		size_t kMax = 1024;
		modelVersion_t modelVersion = modelVersion_t::SIMPLE;
		Models::ModelSharedPtr ptrModel;

		// network variable
		Data::Structure::NetworkSharedPtr ptrNetwork;

		// scheduler
		typedef enum {NONE = 0, UPDATE = 1, RESET = 2} schedulerOperation_t;
		schedulerOperation_t schedulerOperation = RESET;
		Likelihood::Scheduler::SchedulerSharedPtr ptrScheduler;

		// approximator
		double initDeltaT = 0.05;
		bool dirtyApproximator = false;
		approximatorVersion_t approxVersion = approximatorVersion_t::DEFAULT;
		Likelihood::Approximator::ApproximatorSharedPtr ptrApproximator;

		// integration schemes
		integrationScheme_t integrationScheme = integrationScheme_t::RUNGE_KUTTA_DOPRI5;


};

} // end namespace interface
} // end namespace diversinet

#endif // _DIVNET_INTERFACE_
