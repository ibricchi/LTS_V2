#include <string>
#include <vector>
#include <CustomExceptionClasses/unsupportedIntegrationMethodOrderException.hpp>

#include "diode.hpp"

Diode::Diode(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    int n1 = stoi(args[0]);
    int n2 = stoi(args[1]);
    float val = getValue(args[2]);
	int order = 1;

	subComponents = 2;
	nodes.push_back(n1);
	nodes.push_back(n2);	
	compCurrent = 0;
	
	types.push_back(componentType::conductanceSource);
	types.push_back(componentType::currentSource);
	// types.push_back(componentType::vcUpdatable);
}

Diode::Diode(string _name,float c, int n1, int n2, float timeStep, int order)
	:Component{_name}, capacitance{c}{
	subComponents = 2;	
	nodes.push_back(n1);
	nodes.push_back(n2);	
	compCurrent = 0;

	types.push_back(componentType::conductanceSource);
	types.push_back(componentType::currentSource);
	// types.push_back(componentType::vcUpdatable);
}

float Diode::getCurrent() const{
	return compCurrent;
}

// float Diode::getTotalCurrent(float voltage, int order){
	
// 	throw unsupportedIntegrationMethodOrderException();

// }

// void Diode::updateVals(float newVoltage, float newCurrent, int order){
// 		throw unsupportedIntegrationMethodOrderException();
// }

vector<int> Diode::getNodes() const{
    vector<int> res{};
    res.push_back(nodes.at(0));
    res.push_back(nodes.at(1));
    return res;
}
