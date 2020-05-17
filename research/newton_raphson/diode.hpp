#ifndef DIODE_HPP
#define DIODE_HPP

#include "component.hpp"

class Diode: public Component{
private:
	static const float vt = 25e-3; //Constants for now. Could determine from netlist later on
	static const float is = 10e-14;
	float compCurrent;//Value of the current source in the companion model
	// float compVoltage; //value of the voltage across the diode
	float compConductance;//Conductance of the resistor in the companion model

public:
	Diode(string name, vector<string> args, vector<float> extraInfo);
	Diode(string _name, float l, int n1, int n2, float timeStep, int order);
	~Diode() = default;
	// float getIs() const;
	// float getVt() const;
	vector<int> getNodes() const ;
	float getCurrent() const override;
	// float getTotalCurrent(float voltage, int order = 1) override;
	// void updateVals(float newVoltage, float newCurrent, int order) override; //Called at the end of every iteration, after calculating node voltages and component currents.

	//voltage = voltage across diode (v_n1 - v_n2)
	void updateVals(float time_or_voltage) override;
};

#endif
