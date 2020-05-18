#ifndef CURRENTCONTROLLEDSOURCE_HPP
#define CURRENTCONTROLLEDSOURCE_HPP

#include <vector>
#include <string>

#include "component.hpp"

/*
    Acts as interface for current controlled sources
*/
class CurrentControlledSource: public Component{
protected:
    double gain;
    string vsName; //name of the voltage source whose current is the controlling current

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