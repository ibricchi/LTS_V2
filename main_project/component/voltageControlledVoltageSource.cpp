#include <string>
#include <iostream>

#include "voltageControlledVoltageSource.hpp"

VoltageControlledVoltageSource::VoltageControlledVoltageSource(string name, vector<string> args, vector<float> extraInfo)
    :VoltageControlledSource(name, args, extraInfo)
{
    voltageSourcesIdx = extraInfo[2];
    types.push_back(componentType::voltageSource);
}

double VoltageControlledVoltageSource::ivAtNode(int n) const{
    cerr << "ivAtNode not implemented in dependent voltage source" <<endl;
    exit(1);
}
double VoltageControlledVoltageSource::divAtNode(int n, int dn) const{
    cerr << "divAtNode not implemented in dependent voltage source" <<endl;
    exit(1);
}

string VoltageControlledVoltageSource::getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    return to_string(x(highestNodeNumber+voltageSourcesIdx));
}