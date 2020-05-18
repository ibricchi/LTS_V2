#include <string>
#include <iostream>

#include "currentControlledSource.hpp"

CurrentControlledSource::CurrentControlledSource(string name, vector<string> args, vector<double> extraInfo)
    :Component{name}
{
    int n1 = stoi(args[0]);
    int n2 = stoi(args[1]);

    string vsName = args[2];

    setupBasic(n1, n2, vsName);

    gain = stod(args[4]);
}

// CurrentControlledSource::CurrentControlledSource(string _name, double _voltage, int n1, int n2)
//     :Component{_name}{
//     setupBasic(n1, n2);
// }

CurrentControlledSource::~CurrentControlledSource(){}

void CurrentControlledSource::setupBasic(int n1, int n2, string _vsName){
    nodes.push_back(n1);
    nodes.push_back(n2);

    vsName = _vsName;
}

double CurrentControlledSource::getGain() const{
    return gain;
}

string CurrentControlledSource::getVsName() const{
    return vsName;
}

vector<int> CurrentControlledSource::getNodes() const{
    // vector<int> res{};
    // res.push_back(nodes.at(0));
    // res.push_back(nodes.at(1));
    // res.push_back(nodes.at(2));
    // res.push_back(nodes.at(3));
    // return res;
    return nodes;
}