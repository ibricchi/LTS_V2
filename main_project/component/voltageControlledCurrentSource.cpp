#include <string>
#include <iostream>

#include "voltageControlledCurrentSource.hpp"

VoltageControlledCurrentSource::VoltageControlledCurrentSource(string name, vector<string> args, vector<float> extraInfo)
    :VoltageControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::currentSource);
}

float VoltageControlledCurrentSource::ivAtNode(int n) const{
    return 0;
}
float VoltageControlledCurrentSource::divAtNode(int n, int dn) const{
    return 0;
}

float VoltageControlledCurrentSource::getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    float controllingVoltage = nodalVoltages[2] - nodalVoltages[3];
    
    return gain*controllingVoltage;
}