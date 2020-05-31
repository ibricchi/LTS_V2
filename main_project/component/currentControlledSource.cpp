#include <string>
#include <iostream>

#include "currentControlledSource.hpp"

CurrentControlledSource::CurrentControlledSource(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    nodes = processNodes({args[0], args[1]});

    string vsName = args[2];

    gain = Component::getValue(args[3]);
}

CurrentControlledSource::~CurrentControlledSource(){}

float CurrentControlledSource::getGain() const{
    return gain;
}

string CurrentControlledSource::getVsName() const{
    return vsName;
}

vector<int> CurrentControlledSource::getNodes() const{
    return nodes;
}