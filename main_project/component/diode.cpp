#include <string>
#include <vector>
#include <cmath>

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

float Diode::ivAtNode(int n) const{
    float v = nodalVoltages[0] - nodalVoltages[1];
    double current = IS * (exp(v/N/VT) - 1);
    if(n == nodes[1]){
        current*=-1;
    }
    return (float)current;
};

float Diode::divAtNode(int n, int dn) const{
    float v = nodalVoltages[0] - nodalVoltages[1];
    double conductance = IS / N / VT * exp(v/N/VT);
    
    if(n == nodes[0]){
        if(dn == nodes[1]){
            conductance *= -1;
        }
    }else{
        conductance *= -1;
        if(dn == nodes[1]){
            conductance*=-1;
        }
    }
    return (float) conductance;
};

vector<int> Diode::getNodes() const{
    return nodes;
}