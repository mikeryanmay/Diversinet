/*
 * SimpleTests.cpp
 *
 *  Created on: Apr 22, 2023
 *      Author: mike
 */

#include "Test/Catch2/catch.hpp"
#include <Diversinet/DiversinetInterface.h>
#include <cmath>

static std::string yule_newick             = "((A:0.5,B:0.5):0.25,C:0.75):0.25;";
static std::string yule_newick_asym        = "((A:0.5,C#A:0.00):0.25,(B:0.5)C#A:0.25):0.25;";
static std::string yule_newick_sym         = "((A:0.5,C#S1:0.00)D#S2:0.25,(B:0.5,D#S2:0.00)C#S1:0.25):0.25;";
static std::string yule_newick_hyb_species = "((A:0.5,(B:0.5)D#H:0.0):0.25,(C:0.5,D#H:0.0):0.25):0.25;";
static std::string yule_allopoly_species   = "((A:0.5,(B:0.5)D#P:0.0):0.25,(C:0.5,D#P:0.0):0.25):0.25;";

TEST_CASE("Yule process with no hybridization. [Yule][Analytical]") {

	// make the interface
	Diversinet::Interface::DiversinetInterface interface;

	// set the parameters
	interface.setLambda(0.5);
	interface.setMu(0.0);
	interface.setEta(0.0);
	interface.setZeta(0.0);
	interface.setNu(0.0);
	interface.setPsi(0.0);
	interface.setRho(1.0);
	interface.setKMax(128);

	// read the tree
	interface.readNewick(yule_newick);
	double lnl = interface.computeLogLikelihood();

    // The full density includes lambda for each of the two speciation events.
    REQUIRE(lnl == Approx(-1.1250000000 + 2.0 * std::log(0.5)).margin(1.e-7));

}

TEST_CASE("Yule process with asymmetrical hybridization. [Yule][Analytical]") {

	// make the interface
	Diversinet::Interface::DiversinetInterface interface;

	// set the parameters
	interface.setLambda(0.5);
	interface.setMu(0.0);
	interface.setEta(0.1);
	interface.setZeta(0.0);
	interface.setNu(0.0);
	interface.setPsi(0.0);
	interface.setRho(1.0);
	interface.setKMax(128);

	// read the tree
	interface.readNewick(yule_newick_asym);
	double lnl = interface.computeLogLikelihood();

    // The full density includes lambda for the speciation event.
    REQUIRE(lnl == Approx(-3.9457322736 + std::log(0.5)).margin(1.e-7));

}

TEST_CASE("Yule process with symmetrical hybridization. [Yule][Analytical]") {

	// make the interface
	Diversinet::Interface::DiversinetInterface interface;

	// set the parameters
	interface.setLambda(0.5);
	interface.setMu(0.0);
	interface.setEta(0.0);
	interface.setZeta(0.05);
	interface.setNu(0.0);
	interface.setPsi(0.0);
	interface.setRho(1.0);
	interface.setKMax(128);

	// read the tree
	interface.readNewick(yule_newick_sym);
	double lnl = interface.computeLogLikelihood();

    // The full density includes lambda for the speciation event.
    REQUIRE(lnl == Approx(-3.9082322736 + std::log(0.5)).margin(1.e-7));

}

TEST_CASE("Yule process with hybrid speciation. [Yule][Analytical]") {

	// make the interface
	Diversinet::Interface::DiversinetInterface interface;

	// set the parameters
	interface.setLambda(0.5);
	interface.setMu(0.0);
	interface.setEta(0.0);
	interface.setZeta(0.0);
	interface.setNu(0.075);
	interface.setPsi(0.0);
	interface.setRho(1.0);
	interface.setKMax(128);

	// read the tree
	interface.readNewick(yule_newick_hyb_species);
	double lnl = interface.computeLogLikelihood();

    // The full density includes lambda for the speciation event.
    REQUIRE(lnl == Approx(-3.846517 + std::log(0.5)).margin(1.e-7));

}


TEST_CASE("Yule process with allopolyploid speciation. [Yule][Analytical]") {

	// make the interface
	Diversinet::Interface::DiversinetInterface interface;

	// set the parameters
	interface.setLambda(0.5);
	interface.setMu(0.0);
	interface.setEta(0.0);
	interface.setZeta(0.0);
	interface.setNu(0.0);
	interface.setPsi(0.015);
	interface.setRho(1.0);
	interface.setKMax(128);

	// read the tree
	interface.readNewick(yule_allopoly_species);
	double lnl = interface.computeLogLikelihood();

    // The full density includes lambda for the speciation event.
    REQUIRE(lnl == Approx(-5.3509550779 + std::log(0.5)).margin(1.e-7));

}

TEST_CASE("Yule process with incomplete sampling. [Yule][Analytical]") {

	// make the interface
	Diversinet::Interface::DiversinetInterface interface;

	// set the parameters
	interface.setLambda(0.5);
	interface.setMu(0.0);
	interface.setEta(0.0);
	interface.setZeta(0.0);
	interface.setNu(0.0);
	interface.setPsi(0.0);
	interface.setRho(0.5);
	interface.setKMax(128);

	// read the tree
	interface.readNewick(yule_newick);
	double lnl = interface.computeLogLikelihood();

    // The full density includes lambda for each of the two speciation events.
    REQUIRE(lnl == Approx(-2.1918377947 + 2.0 * std::log(0.5)).margin(1.e-7));

}



TEST_CASE("Birth-death process with incomplete sampling. [BD][Analytical]") {

	// make the interface
	Diversinet::Interface::DiversinetInterface interface;

	// set the parameters
	interface.setLambda(0.5);
	interface.setMu(0.1);
	interface.setEta(0.0);
	interface.setZeta(0.0);
	interface.setNu(0.0);
	interface.setPsi(0.0);
	interface.setRho(0.5);
	interface.setKMax(128);

	// read the tree
	interface.readNewick(yule_newick);
	double lnl = interface.computeLogLikelihood();

    // The full density includes lambda for each of the two speciation events.
    REQUIRE(lnl == Approx(-2.3702210866 + 2.0 * std::log(0.5)).margin(1.e-7));

}
