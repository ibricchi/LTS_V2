#ifndef WAVEFORM_HPP
#define WAVEFORM_HPP

#include <cmath>
#include <vector>
#include <string>
#include <map>

#include "component.hpp"
#include "enums.hpp"

using namespace std;

class Waveform{
private:
    // just for convenience
    static constexpr double PI = 3.1415926535897;

    // important
    sourceTypes sourceType;

    //used for sine
    float offset, amplitude, frequency, timeDelay, dampingFactor, phase;

    //used for PWL
    map<float, float> pwlTimeVoltageMapping;

    //used for pulse
    float initialVoltage, peakVoltage, initialDelayTime, riseTime, fallTime, pulseWidth, period;
    
    /*
        SINE netlist syntax:
        Vname N+ N- SIN(VO VA fo <TD> <a> <PHASE>)

        VO - offset voltage in volts.
        VA - amplitude in volts.
        fo -the frequency in hertz.
        TD - delay in seconds
        a - damping factor per second
        Phase - phase in degrees

        Arguments in <> are optional and assumed to be 0 by default
    */
    void setupSin(float startTime, float _voltageOffset, float _voltageAmplitude, float _frequency, float _timeDelay = 0, float _dampingFactor = 0, float _phase = 0);

    /*
        PWL netlist syntax:
        Vname N+ N- PWL(T1 V1 T2 V2 T3 V3 ... Tn Vn)

        (Ti, Vi) specifies the source value Vi at time Ti
    */
    void setupPwl(float startTime, map<float, float> _pwlTimeVoltageMapping);

    /*
        PULSE netlist syntax:
        Vname N+ N- PULSE(Vo V1 Td Tr Tf Tw To)

        Vo - initial voltage
        V1 - peak voltage
        Td - initial delay time
        Tr - rise time
        Tf - fall time
        Tw - pulse-width
        To - period of wave
    */
    void setupPulse(float startTime, float _initialVoltage, float _peakVoltage, float _initialDelayTime, float _riseTime, float _fallTime, float _pulseWidth, float _period);

public:
    void setupWaveform(const Component* comp, vector<string> args, vector<float> extraInfo);

    float updateVals(float time);

    float updateSinVals(float time);
    float updatePwlVals(float time);
    float updatePulseVals(float time);
};

#endif