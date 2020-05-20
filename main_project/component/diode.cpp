#include <string>
#include <vector>
#include <cmath>

#include "diode.hpp"

using namespace std;

Diode::Diode(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    int n1 = stoi(args[0]);
    int n2 = stoi(args[1]);

    nodalVoltages = {0,0};

    nodes.push_back(n1);
    nodes.push_back(n2);

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

float Diode::ivAtNode(int n1) const{
    float v = nodalVoltages[0] - nodalVoltages[1];
    double current = IS * (exp(v/N/VT) - 1);
    current *= n1==nodes[0]?1:-1;
    return (float)current;
};

float Diode::divAtNode(int n1, int dn) const{
    float v = nodalVoltages[0] - nodalVoltages[1];
    double conductance = IS / N / VT * exp(v/N/VT);
    conductance *= n1==nodes[0]?1:-1 * dn==nodes[0]?1:-1;
    return (float) conductance;
};

vector<int> Diode::getNodes() const{
    return nodes;
}