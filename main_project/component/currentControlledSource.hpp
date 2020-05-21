#ifndef CURRENTCONTROLLEDSOURCE_HPP
#define CURRENTCONTROLLEDSOURCE_HPP

#include <vector>
#include <string>

#include "component.hpp"

/*
    Acts as interface for current controlled sources
    
    MNA only calculates currents through voltage sources.
    Hence, controlling current must be the current through one of the voltage sources.
    If a controlling current that doesn't initially go through a voltage source is required, a zero voltage source must be added to the circuit's netlist
    (Then current through this source will be calculated).
*/
class CurrentControlledSource: public Component{
protected:
    double gain;

    //Name of the voltage source whose current is the controlling current.
    //vsName is case sensitive and must match the name of the voltage source exactly.
    //The case sensitivity was added on purpose as otherwise we would have to iterate over each character everytime that matrix A is constructed.
    string vsName;

public:
    CurrentControlledSource(string _name, vector<string> args, vector<double> extraInfo);
    // CurrentControlledSource(string _name, double _voltage, int n1, int n2);
    virtual ~CurrentControlledSource() = 0;

    void setupBasic(int n1, int n2, string vsName);

    double getGain() const override;
    string getVsName() const override;

    vector<int> getNodes() const override;
};

#endif