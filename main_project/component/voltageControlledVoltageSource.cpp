#include <string>
#include <iostream>

#include "voltageControlledVoltageSource.hpp"

VoltageControlledVoltageSource::VoltageControlledVoltageSource(string name, vector<string> args, vector<float> extraInfo)
    :VoltageControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::voltageSource);
}

float VoltageControlledVoltageSource::ivAtNode(int n) const{
    cerr << "ivAtNode not implemented in dependent source" <<endl;
    exit(1);
}
float VoltageControlledVoltageSource::divAtNode(int n, int dn) const{
    cerr << "divAtNode not implemented in dependent source" <<endl;
    exit(1);
}