#ifndef CURRENTCONTROLLEDVOLTAGESOURCE_HPP
#define CURRENTCONTROLLEDVOLTAGESOURCE_HPP

#include <vector>
#include <string>

#include "currentControlledSource.hpp"

class CurrentControlledVoltageSource: public CurrentControlledSource{
public:
    CurrentControlledVoltageSource(string _name, vector<string> args, vector<float> extraInfo);
    ~CurrentControlledVoltageSource() = default;
};

#endif