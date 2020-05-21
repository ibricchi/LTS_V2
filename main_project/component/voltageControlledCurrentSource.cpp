#include <string>
#include <iostream>

#include "voltageControlledCurrentSource.hpp"

VoltageControlledCurrentSource::VoltageControlledCurrentSource(string name, vector<string> args, vector<float> extraInfo)
    :VoltageControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::currentSource);
}

float VoltageControlledCurrentSource::ivAtNode(int n) const{
    cerr << "ivAtNode not implemented in dependent source" <<endl;
    exit(1);
}
float VoltageControlledCurrentSource::divAtNode(int n, int dn) const{
    cerr << "divAtNode not implemented in dependent source" <<endl;
    exit(1);
}