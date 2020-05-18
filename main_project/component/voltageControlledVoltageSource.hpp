#ifndef VOLTAGECONTROLLEDVOLTAGESOURCE_HPP
#define VOLTAGECONTROLLEDVOLTAGESOURCE_HPP

#include <vector>
#include <string>

#include "controlledSource.hpp"

class VoltageControlledVoltageSource: public ControlledSource{
public:
    VoltageControlledVoltageSource(string _name, vector<string> args, vector<double> extraInfo);
    VoltageControlledVoltageSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2);
    ~VoltageControlledVoltageSource() = default;
};

#endif