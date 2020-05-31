#include <string>
#include <vector>

#include "resistor.hpp"

Resistor::Resistor(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    nodes = processNodes({args[0], args[1]});

    nodalVoltages = {0, 0};

    float val = getValue(args[2]);
    conductance = 1/val;

    types.push_back(componentType::conductanceSource);
	types.push_back(componentType::nonVoltageSource);
}

float Resistor::getConductance() const{
    return conductance;
}

// get the IV characteristics of a component given two nodes
double Resistor::ivAtNode(int n1) const{
    return 0;
}

// get the derivative of the IV charateristic of a component given two nodes and the node the derivative is based on
double Resistor::divAtNode(int n1, int dn) const{
    return conductance * (n1==nodes[0]?-1:1) * (dn==nodes[0]?-1:1);
}

float Resistor::getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    voltage = nodalVoltages[0] - nodalVoltages[1];

    return conductance*voltage;
}