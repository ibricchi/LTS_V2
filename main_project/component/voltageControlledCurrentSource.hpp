#ifndef VOLTAGECONTROLLEDCURRENTSOURCE_HPP
#define VOLTAGECONTROLLEDCURRENTSOURCE_HPP

#include <vector>
#include <string>

#include "voltageControlledSource.hpp"

class VoltageControlledCurrentSource: public VoltageControlledSource{
public:
    VoltageControlledCurrentSource(string _name, vector<string> args, vector<float> extraInfo);
    ~VoltageControlledCurrentSource() = default;

    double ivAtNode(int n) override;
    double divAtNode(int n, int dn) const override;

    string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif