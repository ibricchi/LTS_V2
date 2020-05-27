#include <string>
#include <vector>
#include <CustomExceptionClasses/unsupportedIntegrationMethodOrderException.hpp>

#include "capacitor.hpp"

#include <iostream> //testing

Capacitor::Capacitor(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
	nodes = processNodes({args[0], args[1]});

	nodalVoltages = {0,0};

    float val = getValue(args[2]);
	int order = 1;

	subComponents = 2;
	compCurrent = 0;
	prevCurrent = 0; // previous comp_current
	prevVoltage = 0;
	prevTotalCurrent =0;

	capacitance = val;
	timeStep = extraInfo[0];
	
	if(order==1){ //Conductance of the capacitor will be the same as the companion model even at T=0 
		compConductance = (2.0f*capacitance)/timeStep;
	}else{
		throw UnsupportedIntegrationMethodOrderException("capacitor.cpp/constructor");
	}

	types.push_back(componentType::conductanceSource);
	types.push_back(componentType::currentSource);
	types.push_back(componentType::vcUpdatable);
	types.push_back(componentType::nonVoltageSource);
}

float Capacitor::getConductance() const{
	return compConductance;
}

float Capacitor::getCurrent() const{
	return compCurrent;
}

float Capacitor::getTotalCurrent(const VectorXd &x, float voltage, int order) {
	voltage = nodalVoltages[0] - nodalVoltages[1];
	
	if(order == 1){ //companion model from Trapezoidal numerical integration method
		float res= voltage*compConductance - compConductance*compVoltage - prevTotalCurrent;
		prevTotalCurrent = res;
		return res;	
	}else{
		throw UnsupportedIntegrationMethodOrderException("capacitor.cpp/getTotalCurrent");
	}
}

void Capacitor::updateVals(float newVoltage, float newCurrent, int order){
	if(order==1){ //using companion model for the trapezoid integration method.
		// newCurrent = (comp_conductance*newVoltage) - comp_current; //Current into capacitor = current through companion conductance - (as current source pointing towards + node) current source current.		
		// comp_current = -comp_conductance*newVoltage - newCurrent;
		// comp_current = comp_conductance*newVoltage-comp_conductance*prev_voltage - comp_current;
		//comp_current = comp_conductance * newVoltage;		
		compCurrent = (2.0*compConductance*newVoltage) - compCurrent; //From trapezoid companion circuit diagram for capacitor. newVoltage = Vn, 		
		//prev_voltage = newVoltage;		
	//	prev_current = comp_current;		
		compVoltage = newVoltage;
	}else{
		throw UnsupportedIntegrationMethodOrderException("capacitor.cpp/updateVals");
	}	
}

void Capacitor::setTimeStep(float _timeStep){
	timeStep = _timeStep;
	
	//use the new timeStep to update conductance
	compConductance = (2.0f*capacitance)/timeStep;
}

vector<int> Capacitor::getNodes() const{
    vector<int> res{};
    res.push_back(nodes.at(0));
    res.push_back(nodes.at(1));
    return res;
}

float Capacitor::ivAtNode(int n) const{
	return 1;
}
float Capacitor::divAtNode(int n, int dn) const{
	return 1;
}