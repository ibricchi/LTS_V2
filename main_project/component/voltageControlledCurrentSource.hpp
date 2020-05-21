#ifndef VOLTAGECONTROLLEDCURRENTSOURCE_HPP
#define VOLTAGECONTROLLEDCURRENTSOURCE_HPP

#include <vector>
#include <string>

#include "voltageControlledSource.hpp"

class VoltageControlledCurrentSource: public VoltageControlledSource{
public:
    VoltageControlledCurrentSource(string _name, vector<string> args, vector<double> extraInfo);
    // VoltageControlledVoltageSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2);
    ~VoltageControlledCurrentSource() = default;
};

#endif