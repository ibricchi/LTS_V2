#ifndef VOLTAGESOURCE_HPP
#define VOLTAGESOURCE_HPP

#include <vector>
#include <string>

#include "component.hpp"
#include "waveform.hpp"

class VoltageSource: public Component{
private:
    int sourceType;

    //used for dc
    double voltage;

    Waveform voltageWaveform{};
public:
    VoltageSource(string _name, vector<string> args, vector<double> extraInfo);
    VoltageSource(string _name, double _voltage, int n1, int n2);
    ~VoltageSource() = default;

    void setupBasic(int n1, int n2);
    void setupDC(double voltage);

    void updateVals(double time) override;

    double getVoltage() const override;

    vector<int> getNodes() const override;
};

#endif