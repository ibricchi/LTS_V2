#include <string>
#include <iostream>

#include "voltageControlledVoltageSource.hpp"

VoltageControlledVoltageSource::VoltageControlledVoltageSource(string name, vector<string> args, vector<float> extraInfo)
    :VoltageControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::voltageSource);
}