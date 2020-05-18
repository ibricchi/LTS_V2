#include <string>
#include <iostream>

#include "currentControlledVoltageSource.hpp"

CurrentControlledVoltageSource::CurrentControlledVoltageSource(string name, vector<string> args, vector<double> extraInfo)
    :ControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::voltageSource);
}

// CurrentControlledVoltageSource::CurrentControlledVoltageSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2)
//     :ControlledSource(_name, _voltage, n1, n2, nc1, nc2)
// {
//     types.push_back(componentType::voltageSource);
// }