#ifndef NMOSFET_HPP
#define NMOSFET_HPP

#include "component.hpp"
#include "enums.hpp"

/*
    Full netlist => M<name> <nd> <ng> <ns> <nb> <model-name> [L=value] [W=value]
	Where model name must be defined seperately and is associated params such as nmos/pmos and Vt.
    Arguments L and W are optional
	Currently only a simplified netlist is supported => M<name> <nd> <ng> <ns> <nb>
*/
class Mosfet: public Component{
private:
    //Constants for now. Should determine from netlist .options statement later on
	static constexpr double vt = 2; 
	static constexpr double va = 100;
	static constexpr double k = 0.25e-3; //calculate in constructor from .options arguments (e.g length, width)
    static constexpr mosfetType type = mosfetType::NMOS;
	
    double gain; //value of the voltage controlled current source in the companion model
    double compCurrent; //Value of the current source in the companion model
	double compConductance; //Conductance of the resistor in the companion model (keep this name to support polymorthism)
	double gm; //value of the gm conductance

public:
	Mosfet(string name, vector<string> args, vector<double> extraInfo);
	~Mosfet() = default;

	vector<int> getNodes() const;
	double getConductance() const override;
	double getGm() const override;
	double getCurrent() const override;
	double getGain() const override;
	double getTotalCurrent(double voltage, int order = 1) override;
	
	void updateVals(double vgs, double vds) override;

	// helper function to determine the current operating region of the mosfet
	operatingRegion getOperatingRegion(double vgs, double vds) const;
};

#endif
