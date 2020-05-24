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