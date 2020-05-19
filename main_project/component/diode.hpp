#ifndef DIODE_HPP
#define DIODE_HPP

#include "component.hpp"

/*
	Full netlist => D<name> <n+> <n-> <model-name>
	Where model name must be defined seperately and is associated params such as Is.
	Currently only a simplified netlist is supported => D<name> <n+> <n-> 
*/
class Diode: public Component{
private:
	//Constants for now. Should determine from netlist .options statement later on
	static constexpr double vt = 25e-3f;
	static constexpr double is = 1e-14f;
	
	double compCurrent; //Value of the current source in the companion model
	// double compVoltage; //value of the voltage across the diode
	double compConductance; //Conductance of the resistor in the companion model

	double prevVoltage; //used for newton-raphson convergence checking
public:
	Diode(string name, vector<string> args, vector<double> extraInfo);
	Diode(string _name, double l, int n1, int n2, double timeStep, int order);
	~Diode() = default;

	// double getIs() const;
	// double getVt() const;
	vector<int> getNodes() const ;
	double getConductance() const override;
	double getCurrent() const override;
	double getTotalCurrent(double voltage, int order = 1) override;
	vector<double> getPrevVoltages() const override;

	//voltage = voltage across diode (v_n1 - v_n2)
	void updateVals(double time_or_voltage) override;
};

#endif
