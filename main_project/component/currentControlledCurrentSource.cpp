#include <string>
#include <iostream>

#include "currentControlledCurrentSource.hpp"

CurrentControlledCurrentSource::CurrentControlledCurrentSource(string name, vector<string> args, vector<float> extraInfo)
    :CurrentControlledSource(name, args, extraInfo)
{
    types.push_back(componentType::currentSource);
}

float CurrentControlledCurrentSource::ivAtNode(int n) const{
    cerr << "ivAtNode not implemented in dependent source" <<endl;
    exit(1);
}
float CurrentControlledCurrentSource::divAtNode(int n, int dn) const{
    cerr << "divAtNode not implemented in dependent source" <<endl;
    exit(1);
}