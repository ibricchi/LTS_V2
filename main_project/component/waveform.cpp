#include <iostream>
#include <algorithm>
#include <map>

#include "component.hpp"
#include "waveform.hpp"

void Waveform::setupWaveform(const Component* comp, vector<string> args, vector<float> extraInfo){
    string flow = args[2];

    string typeName = flow.substr(0,3);
    for_each(typeName.begin(), typeName.end(), [](char &c){
	    c = toupper(c);
    });

    if(typeName == "SIN"){
        switch (args.size()) // switch statment depending on how many arguments were given as some are not required
        {
        case 5:
            setupSin(
                extraInfo[1], // start time
                comp->getValue(flow.substr(4, flow.size()-4)), // voltage offset
                comp->getValue(args[3]), // voltage amplitude
                comp->getValue(args[4].substr(0,args[4].size()-1)) // frequency
            );
            break;
        case 6:
            setupSin(
                extraInfo[1], // start time
                comp->getValue(flow.substr(4, flow.size()-4)), // voltage offset
                comp->getValue(args[3]), // voltage amplitude
                comp->getValue(args[4]), // frequency
                comp->getValue(args[5].substr(0,args[5].size()-1)) // time delay
            );
            break;
        case 7:
            setupSin(
                extraInfo[1], // start time
                comp->getValue(flow.substr(4, flow.size()-4)), // voltage offset
                comp->getValue(args[3]), // voltage amplitude
                comp->getValue(args[4]), // frequency
                comp->getValue(args[5]), // time delay
                comp->getValue(args[6].substr(0,args[6].size()-1)) // damping factor
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
            cerr << "wrong number of arguments given for SIN waveform input" <<endl;
            exit(1);
            break;
        }
    }else if(typeName == "PWL"){
        if(args.size()%2 != 0){
            cerr << "Wrong number of arguments given for PWL waveform input. An even number of inputs is required." <<endl;
            exit(1);
        }

        map<float, float> inputPairs{};
        float time{}, value{};
        for(int i{2}; i<args.size(); i+=2){
            //check if first or last param (need to remove bracket)
            if(i == 2){
                time = comp->getValue(flow.substr(4, flow.size()-4));
                value = comp->getValue(args.at(i+1));
            }else if(i == args.size()-2){
                time = comp->getValue(args.at(i));
                value = comp->getValue(args.at(i+1).substr(0,args.at(i+1).size()-1));
            }else{
                time = comp->getValue(args.at(i));
                value = comp->getValue(args.at(i+1));
            }
            inputPairs.emplace(time, value);
        }

        setupPwl(
            extraInfo[1], // start time
            inputPairs // time-voltage mapping
        );
    }else if(typeName == "PULSE"){
        if(args.size() != 9){
            cerr << "Wrong number of arguments given for PULSE waveform input. All 9 arguments are required." <<endl;
            exit(1);
        }

        setupPulse(
            extraInfo[1], // start time
            comp->getValue(flow.substr(4, flow.size()-4)), // initial voltage
            comp->getValue(args[3]), // peak voltage
            comp->getValue(args[4]), // initial delay time
            comp->getValue(args[5]), // rise time
            comp->getValue(args[6]), // fall time
            comp->getValue(args[7]), // pulse-width
            comp->getValue(args[8].substr(0,args[8].size()-1)) // period of wave
        );
    }else{
        cerr << "Tried to use unsupported waveform type" <<endl;
        exit(1);
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