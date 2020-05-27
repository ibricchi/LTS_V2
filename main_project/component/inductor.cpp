#include <string>
#include <vector>
#include <CustomExceptionClasses/unsupportedIntegrationMethodOrderException.hpp>

#include "inductor.hpp"

Inductor::Inductor(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    nodes = processNodes({args[0], args[1]});

	nodalVoltages = {0,0};

	int order = 1;
	subComponents = 2;
	compCurrent = 0; // Refers to the current of the companion model current source, however is used opposite in direction to standard current source convention in updateVals for simplicity of coding. getCurrent returns the correct value and direction.
	prevTotalCurrent=0;
	compVoltage = 0; //Refers to voltage across the inductor after the previous timestep's voltage calculation.

	inductance = getValue(args[2]);
	timeStep = extraInfo[0];

	if(order==1){ //Conductance of the inductor will be the same as the companion model even at T=0 
		compConductance = timeStep/(2.0*inductance);
		compCurrent = 
	}else{
		throw UnsupportedIntegrationMethodOrderException("inductor.cpp/constructor");
	}

	types.push_back(componentType::conductanceSource);
	types.push_back(componentType::currentSource);
	types.push_back(componentType::vcUpdatable);
	types.push_back(componentType::nonVoltageSource);
}

float Inductor::getConductance() const{
	return compConductance;
}

float Inductor::getCurrent() const{
	return -compCurrent; //So it's in the right direction, as current source points towards negative.
}

float Inductor::getTotalCurrent(float voltage, int order){
	if(order == 1){ //companion model from Trapezoidal numerical integration method
		float res= voltage*compConductance + compConductance*compVoltage +prevTotalCurrent; //
		prevTotalCurrent = res;
		return res; //negative as current flows from n1 to n2 of inductor
	}else{
		throw UnsupportedIntegrationMethodOrderException("inductor.cpp/getTotalCurrent");
	}
}

void Inductor::updateVals(float newVoltage, float newCurrent, int order){
	if(order==1){ //using companion model for the trapezoid integration method.		
		compCurrent =(2.0*compConductance*newVoltage)+compCurrent;
		compVoltage = newVoltage;
	}else{
		throw UnsupportedIntegrationMethodOrderException("inductor.cpp/updateVals");
	}
}

void Inductor::setTimeStep(float _timeStep){
	timeStep = _timeStep;

	//use the new timeStep to update conductance
	compConductance = timeStep/(2.0*inductance);
}

void Inductor::initCompCurrent(float _current){
compCurrent = _current;
prevTotalCurrent = _current;
}

vector<int> Inductor::getNodes() const{
    vector<int> res{};
    res.push_back(nodes.at(0));
    res.push_back(nodes.at(1));
    return res;
}

float Inductor::ivAtNode(int n) const{
	return 1;
};
float Inductor::divAtNode(int n, int dn) const{
	return 1;
};
