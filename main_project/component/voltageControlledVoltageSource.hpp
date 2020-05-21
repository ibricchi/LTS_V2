#ifndef VOLTAGECONTROLLEDVOLTAGESOURCE_HPP
#define VOLTAGECONTROLLEDVOLTAGESOURCE_HPP

#include <vector>
#include <string>

#include "voltageControlledSource.hpp"

class VoltageControlledVoltageSource: public VoltageControlledSource{
public:
    VoltageControlledVoltageSource(string _name, vector<string> args, vector<float> extraInfo);
    ~VoltageControlledVoltageSource() = default;
};

#endif