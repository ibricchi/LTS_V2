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
    float voltage;

    Waveform voltageWaveform{};
public:
    VoltageSource(string _name, vector<string> args, vector<float> extraInfo);
    ~VoltageSource() = default;

    void setupBasic(int n1, int n2);
    void setupDC(float voltage);

    void updateVals(float time);

    float ivAtNode(int n) const override;
    float divAtNode(int n, int dn) const override;

    float getVoltage() const override;

    vector<int> getNodes() const override;
};

#endif