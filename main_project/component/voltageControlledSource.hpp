#ifndef VOLTAGECONTROLLEDSOURCE_HPP
#define VOLTAGECONTROLLEDSOURCE_HPP

#include <vector>
#include <string>

#include "component.hpp"

/*
    Acts as interface for voltage controlled sources
*/
class VoltageControlledSource: public Component{
protected:
    double gain;

public:
    VoltageControlledSource(string _name, vector<string> args, vector<double> extraInfo);
    // VoltageControlledSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2);
    virtual ~VoltageControlledSource() = 0;

    void setupBasic(int n1, int n2, int nc1, int nc2);

    double getGain() const override;

    //returns all 4 nodes (N+, N-, NC+, NC-)
    vector<int> getNodes() const override;
};

#endif