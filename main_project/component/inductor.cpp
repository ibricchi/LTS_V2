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
	compCurrent = 0;
	prevTotalCurrent=0;

	inductance = getValue(args[2]);
	timeStep = extraInfo[0];

	if(order==1){ //Conductance of the inductor will be the same as the companion model even at T=0 
		compConductance = timeStep/(2.0*inductance);
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
		float res= voltage*compConductance + compConductance*compVoltage +prevTotalCurrent;
		prevTotalCurrent = res;
		return res; //negative as current flows from n1 to n2 of inductor
	}else{
		throw UnsupportedIntegrationMethodOrderException("inductor.cpp/getTotalCurrent");
	}
}

void Inductor::updateVals(float newVoltage, float newCurrent, int order){
	if(order==1){ //using companion model for the trapezoid integration method.
		//newCurrent = (comp_conductance*newVoltage) + comp_current; //Current into inductor = current through conductance + current source current		
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