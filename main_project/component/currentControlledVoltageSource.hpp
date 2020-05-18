#ifndef CURRENTCONTROLLEDVOLTAGESOURCE_HPP
#define CURRENTCONTROLLEDVOLTAGESOURCE_HPP

#include <vector>
#include <string>

#include "currentControlledSource.hpp"

/*
    MNA only calculates currents through voltage sources.
    Hence, controlling current must be the current through one of the voltage sources.
    If a controlling current that doesn't initially go through a voltage source is required, a zero voltage source must be added to the circuit's netlist
    (Then current through this source will be calculated).
*/
class CurrentControlledVoltageSource: public CurrentControlledSource{
public:
    CurrentControlledVoltageSource(string _name, vector<string> args, vector<double> extraInfo);
    // CurrentControlledVoltageSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2);
    ~CurrentControlledVoltageSource() = default;
};

#endif