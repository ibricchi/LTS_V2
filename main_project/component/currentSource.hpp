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
    double current;

    Waveform currentWaveform{};
public:
    CurrentSource(string _name, vector<string> args, vector<double> extraInfo);
    CurrentSource(string _name, double _current, int n1, int n2);
    ~CurrentSource() = default;

    void setupBasic(int n1, int n2);
    void setupDC(double current);

    void updateVals(double time) override;

    double getCurrent() const override;

    vector<int> getNodes() const override;
};

#endif