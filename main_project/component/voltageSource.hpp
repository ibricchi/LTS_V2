#ifndef VOLTAGESOURCE_HPP
#define VOLTAGESOURCE_HPP

#include <vector>
#include <string>

#include "component.hpp"
#include "waveform.hpp"

class VoltageSource: public Component{
private:
    int sourceType;

    //index of the voltageSource inside the voltageSources vector
    //used for getTotalCurrentString
    int voltageSourcesIdx;

    //used for dc
    float voltage;

    //used for ac
    Waveform voltageWaveform{};
public:
    VoltageSource(string _name, vector<string> args, vector<float> extraInfo);
    ~VoltageSource() = default;

    void setupBasic();
    void setupDC(float voltage);

    void updateVals(float time);

    double ivAtNode(int n) override;
    double divAtNode(int n, int dn) override;

    float getVoltage() const override;

    vector<float> getTimePoints();

    string getTotalCurrentString(const VectorXd &x, int highestNodeNumber,bool overwrite=1, float voltage = 0, int order = 1)  override;
};

#endif
