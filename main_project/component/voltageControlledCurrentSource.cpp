#include <string>
#include <iostream>

#include "voltageControlledCurrentSource.hpp"

VoltageControlledCurrentSource::VoltageControlledCurrentSource(string name, vector<string> args, vector<float> extraInfo)
    :VoltageControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::currentSource);
}

double VoltageControlledCurrentSource::ivAtNode(int n){
    return 0;
}
double VoltageControlledCurrentSource::divAtNode(int n, int dn){
    return 0;
}

string VoltageControlledCurrentSource::getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    float controllingVoltage = nodalVoltages[2] - nodalVoltages[3];
    
    return to_string(gain*controllingVoltage);
}