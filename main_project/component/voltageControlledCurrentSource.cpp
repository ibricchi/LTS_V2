#include <string>
#include <iostream>

#include "voltageControlledCurrentSource.hpp"

VoltageControlledCurrentSource::VoltageControlledCurrentSource(string name, vector<string> args, vector<double> extraInfo)
    :VoltageControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::currentSource);
}

// VoltageControlledCurrentSource::VoltageControlledCurrentSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2)
//     :VoltageControlledCurrentSource(_name, _voltage, n1, n2, nc1, nc2)
// {
//     types.push_back(componentType::voltageSource);
// }