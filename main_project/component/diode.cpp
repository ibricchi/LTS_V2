#include <string>
#include <vector>
#include <cmath>

#include "diode.hpp"

using namespace std;

Diode::Diode(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    nodes = processNodes({args[0], args[1]});

    //initialize .model name if one exists
    modelName = (args.size()>2) ? args[2] : "";

    setNodalVoltages({0,0});

    // get's minimum pn conductnace from extrainfo
    minPNConductance = extraInfo[3];

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
        case diodeParamType::VTO:
            VT = paramValue;
            break;
    }
}

void Diode::setNodalVoltages(vector<float> nv){
    nodalVoltages = nv;

    v = nodalVoltages[0] - nodalVoltages[1];
    current = IS * (exp(v/N/VT) - 1) * (1-v/VT);
    lastIeq = current;
    
    conductance = (IS / N / VT) * exp(v/N/VT);
    if(conductance < minPNConductance) conductance = minPNConductance;
    lastConductance = conductance;
}

double Diode::ivAtNode(int n){
    return current * (n==nodes[0]?1:-1);
}

double Diode::divAtNode(int n, int dn){
    return conductance * (n!=dn?-1:1);
}

void Diode::setMinPNConductance(float con){
    minPNConductance = con;
}

string Diode::getModelName() const{
    return modelName;
}

string Diode::getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    return to_string(v*lastConductance + lastIeq);
}