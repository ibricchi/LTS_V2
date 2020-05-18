#include <string>
#include <iostream>

#include "currentControlledCurrentSource.hpp"

CurrentControlledCurrentSource::CurrentControlledCurrentSource(string name, vector<string> args, vector<double> extraInfo)
    :CurrentControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::currentSource);
}

// CurrentControlledCurrentSource::CurrentControlledCurrentSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2)
//     :CurrentControlledSource(_name, _voltage, n1, n2, nc1, nc2)
// {
//     types.push_back(componentType::voltageSource);
// }