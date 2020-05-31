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

    switch (args.size())
    {
    case 2:
        SetupValues();
        break;
    case 3:
        SetupValues(
            stof(args[2])
        );
        break;
    case 4:
        SetupValues(
            stof(args[2]),
            stof(args[3])
        );
        break;
    case 5:
        SetupValues(
            stof(args[0]),
            stof(args[1]),
            stof(args[2])
        );
        break;
    default:
        break;
    }
}

void Diode::SetupValues(float _IS, float _N, float _VT){
    IS = _IS;
    N = _N;
    VT = _VT;
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

vector<int> Diode::getNodes() const{
    return nodes;
}

float Diode::getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    //current through current source and current through resistor
    return nanf("");
}