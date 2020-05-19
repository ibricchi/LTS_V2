#include <string>
#include <vector>

#include "diode.hpp"

Diode::Diode(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    int n1 = stoi(args[0]);
    int n2 = stoi(args[1]);

    nodes.push_back(n1);
    nodes.push_back(n2);

    types.push_back(componentType::nonLinear);
}

void Diode::addParam(int paramId, float paramValue){
    switch(static_cast<diodeParamType>(paramId)){ //need this as strongly typed enums don't automatically convert to their underlying type
        case diodeParamType::IS:
            is = paramValue;
            break;
        case diodeParamType::N:
            n = paramValue;
            break;
    }
}

vector<int> Diode::getNodes() const{
    return nodes;
}