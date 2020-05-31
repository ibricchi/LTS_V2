#ifndef CAPACITOR_HPP
#define CAPACITOR_HPP

#include "component.hpp"

class Capacitor: public Component{
private:
	float capacitance;
	float compCurrent;//Value of the current source in the companion model
	float compVoltage; //Voltage across the Capacitor after the previous iteration has finished calculating.
	float compConductance;//Conductance of the resistor in the companion model
	float prevVoltage; 
	float prevCurrent;
	float prevTotalCurrent;
	float timeStep; //used to update compConductance

public:
	Capacitor(string name, vector<string> args, vector<float> extraInfo);
	~Capacitor() = default;
	float getConductance() const override;

	double ivAtNode(int n1) const override;
    double divAtNode(int n1, int dn) const override;

	float getCurrent() const override;
	string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
	void updateVals(float newVoltage, float newCurrent, int order) override; //Called at the end of every iteration, after calculating node voltages and component currents.
	void setTimeStep(float _timeStep) override;
	void initCompCurrent(float _voltage);
};
	

#endif
