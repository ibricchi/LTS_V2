#include <string>
#include <iostream>

#include "currentControlledVoltageSource.hpp"

CurrentControlledVoltageSource::CurrentControlledVoltageSource(string name, vector<string> args, vector<float> extraInfo)
    :CurrentControlledSource(name, args, extraInfo)
{
    voltageSourcesIdx = extraInfo[2];
    types.push_back(componentType::voltageSource);
}

double CurrentControlledVoltageSource::ivAtNode(int n){
    cerr << "ivAtNode not implemented in dependent voltage source" <<endl;
    exit(1);
}
double CurrentControlledVoltageSource::divAtNode(int n, int dn) const{
    cerr << "divAtNode not implemented in dependent voltage source" <<endl;
    exit(1);
}

string CurrentControlledVoltageSource::getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    return to_string(x(highestNodeNumber+voltageSourcesIdx));
}