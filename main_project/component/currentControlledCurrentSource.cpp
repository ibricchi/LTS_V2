#include <string>
#include <iostream>

#include "currentControlledCurrentSource.hpp"

CurrentControlledCurrentSource::CurrentControlledCurrentSource(string name, vector<string> args, vector<float> extraInfo)
    :CurrentControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::currentSource);
}