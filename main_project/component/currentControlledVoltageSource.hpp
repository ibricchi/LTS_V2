#ifndef CURRENTCONTROLLEDVOLTAGESOURCE_HPP
#define CURRENTCONTROLLEDVOLTAGESOURCE_HPP

#include <vector>
#include <string>

#include "currentControlledSource.hpp"

class CurrentControlledVoltageSource: public CurrentControlledSource{
private:
    //index of the voltageSource inside the voltageSources vector
    //used for getTotalCurrentString
    int voltageSourcesIdx;
public:
    CurrentControlledVoltageSource(string _name, vector<string> args, vector<float> extraInfo);
    ~CurrentControlledVoltageSource() = default;

    double ivAtNode(int n) override;
    double divAtNode(int n, int dn) const override;

    string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif