#ifndef CURRENTCONTROLLEDCURRENTSOURCE_HPP
#define CURRENTCONTROLLEDCURRENTSOURCE_HPP

#include <vector>
#include <string>

#include "currentControlledSource.hpp"

class CurrentControlledCurrentSource: public CurrentControlledSource{
public:
    CurrentControlledCurrentSource(string _name, vector<string> args, vector<float> extraInfo);
    ~CurrentControlledCurrentSource() = default;

    float ivAtNode(int n) const override;
    float divAtNode(int n, int dn) const override;

    float getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif