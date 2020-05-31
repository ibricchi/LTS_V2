#include <string>
#include <vector>
#include <cmath>

#include <iostream> //testing only

#include "diode.hpp"

using namespace std;

Diode::Diode(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    nodes = processNodes({args[0], args[1]});

    nodalVoltages = {0,0};

	types.push_back(componentType::nonVoltageSource);
	types.push_back(componentType::nonLinear);
}

void Diode::addParam(int paramId, float paramValue){
    switch(static_cast<diodeParamType>(paramId)){ //need this as strongly typed enums don't automatically convert to their underlying type
        case diodeParamType::IS:
            IS = paramValue;
            break;
        case diodeParamType::N:
            N = paramValue;
            break;
        case diodeParamType::VT:
            VT = paramValue;
            break;
    }
}

double Diode::ivAtNode(int n) const{
    double v = nodalVoltages[0] - nodalVoltages[1];
    double current = (double)IS * (exp(v/N/VT) - 1) * (1-v/VT);
    if(n == nodes[1]){
        current*=-1;
    }
    return current;
}

double Diode::divAtNode(int n, int dn) const{
    double v = nodalVoltages[0] - nodalVoltages[1];
    double conductance = (double)(IS / N / VT) * exp(v/N/VT);
    if(n != dn){
        conductance *= -1;
    }
    return conductance;
}

string Diode::getModelName() const{
    return modelName;
}

float Diode::getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    //current through current source and current through resistor
    return nanf("");
}