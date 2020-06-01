#ifndef EXAMPLEINPUTDIODE_HPP  
#define EXAMPLEINPUTDIODE_HPP

#include <vector>
#include <string>

#include "component.hpp"

/*
    This diode is for testing only.
    It shoud be removed once a real diode is added.

    Netlist syntax => D<name> <node 1> <node 2> <modelName>
    Where modelName = The name of the .model statement.
    modelName is optional.
*/
class ExampleInputDiode: public Component{
private:
    float is; //saturation current
    float n; //emission coefficient
    // float bv; //reverse bias breakdown voltage
    // float ibv; //reverse bias breakdown current
public:
    ExampleInputDiode(string _name, vector<string> args, vector<float> extraInfo);
    
    ~ExampleInputDiode() = default;

    void addParam(int paramId, float paramValue);
    string getModelName() const override;

    double ivAtNode(int n1) override;
    double divAtNode(int n, int dn) override;
};

#endif