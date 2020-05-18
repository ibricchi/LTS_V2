#include <string>
#include <iostream>

#include "voltageControlledSource.hpp"

VoltageControlledSource::VoltageControlledSource(string name, vector<string> args, vector<double> extraInfo)
    :Component{name}
{
    int n1 = stoi(args[0]);
    int n2 = stoi(args[1]);
    int nc1 = stoi(args[2]);
    int nc2 = stoi(args[3]);

    setupBasic(n1, n2, nc1, nc2);

    gain = stod(args[4]);
}

// VoltageControlledSource::VoltageControlledSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2)
//     :Component{_name}{
//     setupBasic(n1, n2, nc1, nc2);
// }

VoltageControlledSource::~VoltageControlledSource(){}

void VoltageControlledSource::setupBasic(int n1, int n2, int nc1, int nc2){
    nodes.push_back(n1);
    nodes.push_back(n2);
    nodes.push_back(nc1);
    nodes.push_back(nc2);
}

double VoltageControlledSource::getGain() const{
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