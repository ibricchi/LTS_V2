#ifndef VOLTAGECONTROLLEDCURRENTSOURCE_HPP
#define VOLTAGECONTROLLEDCURRENTSOURCE_HPP

#include <vector>
#include <string>

#include "voltageControlledSource.hpp"

class VoltageControlledCurrentSource: public VoltageControlledSource{
public:
    VoltageControlledCurrentSource(string _name, vector<string> args, vector<float> extraInfo);
    ~VoltageControlledCurrentSource() = default;
};

#endif