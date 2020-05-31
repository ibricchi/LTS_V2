#ifndef VOLTAGECONTROLLEDVOLTAGESOURCE_HPP
#define VOLTAGECONTROLLEDVOLTAGESOURCE_HPP

#include <vector>
#include <string>

#include "voltageControlledSource.hpp"

class VoltageControlledVoltageSource: public VoltageControlledSource{
private:
    //index of the voltageSource inside the voltageSources vector
    //used for getTotalCurrent
    int voltageSourcesIdx;
public:
    VoltageControlledVoltageSource(string _name, vector<string> args, vector<float> extraInfo);
    ~VoltageControlledVoltageSource() = default;

    double ivAtNode(int n) const override;
    double divAtNode(int n, int dn) const override;

    float getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif