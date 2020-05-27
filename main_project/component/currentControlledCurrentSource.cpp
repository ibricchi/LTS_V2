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
    //need to get controlling current and then mulitply by gain
    return nanf("");
}