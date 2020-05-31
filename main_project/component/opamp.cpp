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

// get the IV characteristics of a component given two nodes
double OpAmp::ivAtNode(int n1){
    cerr << "ivAtNode not implemented for opamp" <<endl;
    exit(1);
}

// get the derivative of the IV charateristic of a component given two nodes and the node the derivative is based on
double OpAmp::divAtNode(int n1, int dn) const{
    cerr << "divAtNode not implemented for opamp" <<endl;
    exit(1);
}

string OpAmp::getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    return to_string(x(highestNodeNumber+voltageSourcesIdx));
}