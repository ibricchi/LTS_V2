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

void Diode::addParam(string paramName, float paramValue){
    
}

vector<int> Diode::getNodes() const{
    return nodes;
}