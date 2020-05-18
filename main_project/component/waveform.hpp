#ifndef WAVEFORM_HPP
#define WAVEFORM_HPP

#include <cmath>
#include <vector>
#include <string>

#include "component.hpp"
#include "enums.hpp"

using namespace std;

class Waveform{
private:
    // just for convenience
    const double PI = 3.1415926535897;

    // important
    enum::sourceTypes sourceType;

    //used for sin
    double offset, amplitude, frequency, timeDelay, dampingFactor, phase;
    
    void setupSin(double startTime, double _voltageOffset, double _voltageAmplitude, double _frequency, double _timeDelay, double _dampingFactor, double _phase);
    double updateSinVals(double time);

public:
    void setupWaveform(const Component* comp, vector<string> args, vector<double> extraInfo);

    double updateVals(double time);
};

#endif