#ifndef CURRENTSOURCE_HPP
#define CURRENTSOURCE_HPP

#include <vector>
#include <string>

#include "component.hpp"
#include "waveform.hpp"

class CurrentSource: public Component{
private:
    int sourceType;

    //used for dc
    float current;

    Waveform currentWaveform{};
public:
    CurrentSource(string _name, vector<string> args, vector<float> extraInfo);
    ~CurrentSource() = default;

    void setupBasic();
    void setupDC(float current);

    void updateVals(float time);

    double ivAtNode(int n1) const override;
    double divAtNode(int n1, int dn) const override;

    float getCurrent() const override;

    vector<int> getNodes() const override;

    float getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif