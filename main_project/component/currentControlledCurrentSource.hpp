#ifndef CURRENTCONTROLLEDCURRENTSOURCE_HPP
#define CURRENTCONTROLLEDCURRENTSOURCE_HPP

#include <vector>
#include <string>

#include "currentControlledSource.hpp"

class CurrentControlledCurrentSource: public CurrentControlledSource{
public:
    CurrentControlledCurrentSource(string _name, vector<string> args, vector<double> extraInfo);
    // CurrentControlledCurrentSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2);
    ~CurrentControlledCurrentSource() = default;
};

#endif