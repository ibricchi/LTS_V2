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

//resistor is a two terminal device that has two nodes
vector<int> Resistor::getNodes() const{
    vector<int> res{};
    res.push_back(nodes.at(0));
    res.push_back(nodes.at(1));
    return res;
}

// get the IV characteristics of a component given two nodes
float Resistor::ivAtNode(int n1) const{
    return conductance * (nodalVoltages[0] - nodalVoltages[1]) * (n1==nodes[0]?1:-1);
};

// get the derivative of the IV charateristic of a component given two nodes and the node the derivative is based on
float Resistor::divAtNode(int n1, int dn) const{
    return conductance * (n1==nodes[0]?-1:1) * (dn==nodes[0]?-1:1);
};