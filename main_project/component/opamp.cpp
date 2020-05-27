#include <string>
#include <vector>
#include <iostream>

#include "opamp.hpp"

OpAmp::OpAmp(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    nodes = processNodes({args[0], args[1], args[2]});
    
    voltageSourcesIdx = extraInfo[2];

    nodalVoltages = {0, 0, 0};

    modelName = args[3]; //Not used for now => Assumed to be ideal

    //Ideal opamp behaves similar to voltage source
	types.push_back(componentType::voltageSource);
}

//resistor is a two terminal device that has two nodes
vector<int> OpAmp::getNodes() const{
    return nodes;
}

// get the IV characteristics of a component given two nodes
float OpAmp::ivAtNode(int n1) const{
    cerr << "ivAtNode not implemented for opamp" <<endl;
    exit(1);
}

// get the derivative of the IV charateristic of a component given two nodes and the node the derivative is based on
float OpAmp::divAtNode(int n1, int dn) const{
    cerr << "divAtNode not implemented for opamp" <<endl;
    exit(1);
}

float OpAmp::getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    return x(highestNodeNumber+voltageSourcesIdx);
}