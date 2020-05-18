#ifndef VOLTAGECONTROLLEDVOLTAGESOURCE_HPP
#define VOLTAGECONTROLLEDVOLTAGESOURCE_HPP

#include <vector>
#include <string>

#include "voltageControlledSource.hpp"

class VoltageControlledVoltageSource: public VoltageControlledSource{
public:
    VoltageControlledVoltageSource(string _name, vector<string> args, vector<double> extraInfo);
    // VoltageControlledVoltageSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2);
    ~VoltageControlledVoltageSource() = default;
};

#endif