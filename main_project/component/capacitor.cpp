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
	// prevCurrent = 0; // previous comp_current
	// prevVoltage = 0;
	prevTotalCurrent =0;

	compCurrent = 0;
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

string Capacitor::getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
	voltage = nodalVoltages[0] - nodalVoltages[1];

	if(order == 1){ //companion model from Trapezoidal numerical integration method
		float res= voltage*compConductance - compConductance*compVoltage - prevTotalCurrent;
		prevTotalCurrent = res;
		
		return to_string(res);	
	}else{
		throw UnsupportedIntegrationMethodOrderException("capacitor.cpp/getTotalCurrentString");
	}
}

void Capacitor::updateVals(float newVoltage, float newCurrent, int order){
	if(order==1){ //using companion model for the trapezoid integration method.
		compCurrent = (2.0f*compConductance*newVoltage) - compCurrent; //From trapezoid companion circuit diagram for capacitor. newVoltage = Vn, 		
			
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


void Capacitor::initCompCurrent(float _voltage){ //_voltage corresponds to the DC bias voltage across the capacitor
	compVoltage = _voltage;
	compCurrent = compConductance * _voltage;
	prevTotalCurrent = 0;
}

double Capacitor::ivAtNode(int n){
	return compCurrent * (n==nodes[0] ? -1 : 1);
}
double Capacitor::divAtNode(int n, int dn) const{
	return compConductance * (n==nodes[0] ? -1 : 1) * (dn==nodes[0] ? -1 : 1);
}
