#include <string>
#include <vector>

// #include <iostream>

#include "exampleInputDiode.hpp"

ExampleInputDiode::ExampleInputDiode(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    int n1 = stoi(args[0]);
    int n2 = stoi(args[1]);
    nodes.push_back(n1);
    nodes.push_back(n2);

    //initialize .model name if one exists
    modelName = (args.size()>2) ? args[2] : "";

    //initialize params to default values
    is = 1e-14;
    n = 1;

    types.push_back(componentType::nonLinear);
}

void ExampleInputDiode::addParam(int paramId, float paramValue){
    switch(static_cast<diodeParamType>(paramId)){ //need this as strongly typed enums don't automatically convert to their underlying type
        case diodeParamType::IS:
            is = paramValue;
            break;
        case diodeParamType::N:
            n = paramValue;
            break;
    }
}

vector<int> ExampleInputDiode::getNodes() const{
    return nodes;
}

string ExampleInputDiode::getModelName() const{
    return modelName;
}

float ExampleInputDiode::ivAtNode(int n1) const{
    return 0.1; //incorrect, just for demonstration
}
float ExampleInputDiode::divAtNode(int n1, int dn) const{
    return 0.1; //incorrect, just for demonstration
}