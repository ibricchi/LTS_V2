#include <string>
#include <iostream>

#include "currentControlledSource.hpp"

CurrentControlledSource::CurrentControlledSource(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    int n1 = stoi(args[0]);
    int n2 = stoi(args[1]);

    string vsName = args[2];

    setupBasic(n1, n2, vsName);

    gain = Component::getValue(args[3]);
}

CurrentControlledSource::~CurrentControlledSource(){}

void CurrentControlledSource::setupBasic(int n1, int n2, string _vsName){
    nodes.push_back(n1);
    nodes.push_back(n2);

    vsName = _vsName;
}

float CurrentControlledSource::getGain() const{
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