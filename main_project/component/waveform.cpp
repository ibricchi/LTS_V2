#include <iostream>

#include "component.hpp"
#include "waveform.hpp"

void Waveform::setupWaveform(const Component* comp, vector<string> args, vector<float> extraInfo){
    string flow = args[2];
    if(flow.substr(0,4) == "SIN(" || flow.substr(0,4) == "sin("){
        switch (args.size()) // switch statment depending on how many arguments were given as some are not required
        {
        case 5:
            setupSin(
                extraInfo[1], // start time
                comp->getValue(flow.substr(4, flow.size()-4)), // voltage offset
                comp->getValue(args[3]), // voltage amplitude
                comp->getValue(args[4].substr(0,args[4].size()-1)), // frequency
                0, 0, 0 // defaults
            );
            break;
        case 6:
            setupSin(
                extraInfo[1], // start time
                comp->getValue(flow.substr(4, flow.size()-4)), // voltage offset
                comp->getValue(args[3]), // voltage amplitude
                comp->getValue(args[4]), // frequency
                comp->getValue(args[5].substr(0,args[5].size()-1)), // time delay
                0, 0 // defaults
            );
            break;
        case 7:
            setupSin(
                extraInfo[1], // start time
                comp->getValue(flow.substr(4, flow.size()-4)), // voltage offset
                comp->getValue(args[3]), // voltage amplitude
                comp->getValue(args[4]), // frequency
                comp->getValue(args[5]), // time delay
                comp->getValue(args[6].substr(0,args[6].size()-1)), // damping factor
                0 // defaults
            );
            break;
        case 8:
            setupSin(
                extraInfo[1], // start time
                comp->getValue(flow.substr(4, flow.size()-4)), // voltage offset
                comp->getValue(args[3]), // voltage amplitude
                comp->getValue(args[4]), // frequency
                comp->getValue(args[5]), // time delay
                comp->getValue(args[6]), // damping factor
                comp->getValue(args[7].substr(0,args[7].size()-1)) // phase
            );
            break;
        default:
            cerr << "wrong number of arguments given for SIN voltage input" <<endl;
            exit(1);
            break;
        }
    }
}


void Waveform::setupSin(float startTime, float _offset, float _amplitude, float _frequency, float _timeDelay, float _dampingFactor, float _phase){
    offset = _offset;
    amplitude = _amplitude;
    frequency = _frequency;
    timeDelay = _timeDelay;
    dampingFactor = _dampingFactor;
    phase = _phase;

    sourceType = sourceTypes::SIN;

    updateSinVals(startTime);
}

void Waveform::setupPwl(float startTime, map<float, float> _pwlTimeVoltageMapping){
   pwlTimeVoltageMapping = _pwlTimeVoltageMapping;

   sourceType = sourceTypes::PWL;

   updatePwlVals(startTime);
}

void Waveform::setupPulse(float startTime, float _initialVoltage, float _peakVoltage, float _initialDelayTime, float _riseTime, float _fallTime, float _pulseWidth, float _period){
    initialVoltage = _initialVoltage;
    peakVoltage = _peakVoltage;
    initialDelayTime = _initialDelayTime;
    riseTime = _riseTime;
    fallTime = _fallTime;
    pulseWidth = _pulseWidth;
    period = _period;

    sourceType = sourceTypes::PULSE;

    updatePulseVals(startTime);
}

float Waveform::updateVals(float time){
    switch(sourceType){
        case sourceTypes::SIN:
            return updateSinVals(time);
            break;
        case sourceTypes::PWL:
            return updatePwlVals(time);
            break;
        case sourceTypes::PULSE:
            return updatePulseVals(time);
            break;
        default:
            cerr << "Tried to use an unknown sourceType in waveforms/updateVals" <<endl;
            exit(1);
            break;
    }
}

float Waveform::updateSinVals(float time){
    return offset + amplitude * exp(-dampingFactor*(time - timeDelay)) * sin(2 * PI * frequency * (time - timeDelay) + (phase/360));
}

float Waveform::updatePwlVals(float time){
    cerr << "PWL not yet supported" <<endl;
    exit(1);
}

float Waveform::updatePulseVals(float time){
    cerr << "Pulse not yet supported" <<endl;
    exit(1);
}