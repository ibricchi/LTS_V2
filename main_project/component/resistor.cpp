#include <string>
#include <vector>

#include "resistor.hpp"

Resistor::Resistor(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    int n1 = stoi(args[0]);
    int n2 = stoi(args[1]);
    float val = getValue(args[2]);

    nodes.push_back(n1);
    nodes.push_back(n2);
    conductance = 1/val;

    types.push_back(componentType::conductanceSource);
	types.push_back(componentType::nonVoltageSource);
}

Resistor::Resistor(string _name, float r, int n1, int n2)
    :Component{_name}, conductance{1/r}{

    nodes.push_back(n1);
    nodes.push_back(n2);
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
float Resistor::ivAtNode(int n1, int n2, float v) const{
    return conductance * v * (n1==nodes[0]?1:-1);
};

// get the derivative of the IV charateristic of a component given two nodes and the node the derivative is based on
float Resistor::divAtNode(int n1, int n2, int dn, float v) const{
    return conductance * (n1==nodes[0]?-1:1) * (n1==nodes[0]?-1:1);
};