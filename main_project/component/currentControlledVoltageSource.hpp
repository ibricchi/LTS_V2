#ifndef CURRENTCONTROLLEDVOLTAGESOURCE_HPP
#define CURRENTCONTROLLEDVOLTAGESOURCE_HPP

#include <vector>
#include <string>

#include "currentControlledSource.hpp"

class CurrentControlledVoltageSource: public CurrentControlledSource{
public:
    CurrentControlledVoltageSource(string _name, vector<string> args, vector<float> extraInfo);
    ~CurrentControlledVoltageSource() = default;

    float ivAtNode(int n) const override;
    float divAtNode(int n, int dn) const override;

    float getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif