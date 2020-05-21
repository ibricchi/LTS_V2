#include <string>
#include <iostream>

#include "currentControlledVoltageSource.hpp"

CurrentControlledVoltageSource::CurrentControlledVoltageSource(string name, vector<string> args, vector<float> extraInfo)
    :CurrentControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::voltageSource);
}