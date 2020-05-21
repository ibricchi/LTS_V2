#include <string>
#include <iostream>

#include "voltageControlledCurrentSource.hpp"

VoltageControlledCurrentSource::VoltageControlledCurrentSource(string name, vector<string> args, vector<float> extraInfo)
    :VoltageControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::currentSource);
}