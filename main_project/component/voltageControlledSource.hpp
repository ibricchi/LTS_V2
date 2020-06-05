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
    float gain;

public:
    VoltageControlledSource(string _name, vector<string> args, vector<float> extraInfo);
    virtual ~VoltageControlledSource() = 0;

    float getGain() const override;
};

#endif