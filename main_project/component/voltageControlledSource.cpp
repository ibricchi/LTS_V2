#include <string>
#include <iostream>

#include "voltageControlledSource.hpp"

VoltageControlledSource::VoltageControlledSource(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    //Order: n1, n2, nc1, nc2
    nodes = processNodes({args[0], args[1], args[3], args[4]});

    gain = Component::getValue(args[4]);
}

VoltageControlledSource::~VoltageControlledSource(){}

float VoltageControlledSource::getGain() const{
    return gain;
}

// 2 output nodes (N+, N-) and 2 controlling nodes (NC+, NC-)
vector<int> VoltageControlledSource::getNodes() const{
    // vector<int> res{};
    // res.push_back(nodes.at(0));
    // res.push_back(nodes.at(1));
    // res.push_back(nodes.at(2));
    // res.push_back(nodes.at(3));
    // return res;
    return nodes;
}