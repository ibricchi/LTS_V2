#include <string>
#include <vector>
#include <cmath>
#include <CustomExceptionClasses/unsupportedIntegrationMethodOrderException.hpp>

#include "diode.hpp"

Diode::Diode(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    int n1 = stoi(args[0]);
    int n2 = stoi(args[1]);
    // float val = getValue(args[2]);

	subComponents = 2;
	nodes.push_back(n1);
	nodes.push_back(n2);	
	
	//initialize conductance/current with an initial guess of vd = 0
	updateVals(0);
	
	types.push_back(componentType::conductanceSource);
	types.push_back(componentType::currentSource);
	types.push_back(componentType::nonLinear);
}

Diode::Diode(string _name,float c, int n1, int n2, float timeStep, int order)
	:Component{_name}{
	subComponents = 2;	
	nodes.push_back(n1);
	nodes.push_back(n2);	

	//initialize conductance/current with an initial guess of vd = 0
	updateVals(0);

	types.push_back(componentType::conductanceSource);
	types.push_back(componentType::currentSource);
	types.push_back(componentType::nonLinear);
}

float Diode::getConductance() const{
	return compConductance;
}

float Diode::getCurrent() const{
	return -compCurrent; //direction of current source is opposite to direction of diode
}

float Diode::getTotalCurrent(float voltage, int order){
	return NAN; //not yet implemented
}

void Diode::updateVals(float time_or_voltage){
	float vd = time_or_voltage;
	float tempExp = exp(vd/vt); //more efficient to only calculate once

	compConductance = is/vt * tempExp;

	compCurrent = is*(tempExp-1) - compConductance*vd;
}

vector<int> Diode::getNodes() const{
    vector<int> res{};
    res.push_back(nodes.at(0));
    res.push_back(nodes.at(1));
    return res;
}