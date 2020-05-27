#include <string>
#include <iostream>

#include "currentControlledCurrentSource.hpp"

CurrentControlledCurrentSource::CurrentControlledCurrentSource(string name, vector<string> args, vector<float> extraInfo)
    :CurrentControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::currentSource);
}

float CurrentControlledCurrentSource::ivAtNode(int n) const{
    return 0;
}
float CurrentControlledCurrentSource::divAtNode(int n, int dn) const{
    return 0;
}

float CurrentControlledCurrentSource::getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    return x(highestNodeNumber+nodes[2]) * gain; //nodes[2] = idx of voltage source that the controlling current flows through
}