#include <string>
#include <iostream>

#include "voltageSource.hpp"

VoltageSource::VoltageSource(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    int n1 = stoi(args[0]);
    int n2 = stoi(args[1]);

    setupBasic(n1, n2);

    if(args.size() == 3){
        float val = getValue(args[2]);
        setupDC(val);
    }else{
        string flow = args[2];
        if(flow == "DC" || flow == "dc" || flow == "Dc" || flow == "dC"){
            setupDC(
                getValue(args[3]) // voltage
            );
        }else if(flow.size() > 4){ //checks if "flow" is long enough to be SIN(* where * is any character
            voltageWaveform.setupWaveform(this, args, extraInfo);

            //initialize voltage value to voltage at start time (extraInfo[1] = startTime)
            updateVals(extraInfo[1]);

            types.push_back(componentType::timeUpdatable);
        }else{
            std::cerr << "Invalid netlist: The syntax of voltage source is incorrect." <<std::endl;
            exit(1);
        }
    }
}

void VoltageSource::setupBasic(int n1, int n2){
    nodes.push_back(n1);
    nodes.push_back(n2);
    
	nodalVoltages = {0,0};

    types.push_back(componentType::voltageSource);
}

void VoltageSource::setupDC(float _voltage){
    voltage = _voltage;
}

float VoltageSource::getVoltage() const{
    return voltage;
}

void VoltageSource::updateVals(float time){
    voltage = voltageWaveform.updateVals(time);
}

//voltageSource is a two terminal device that has two nodes
vector<int> VoltageSource::getNodes() const{
    vector<int> res{};
    res.push_back(nodes.at(0));
    res.push_back(nodes.at(1));
    return res;
}

float VoltageSource::ivAtNode(int n) const{
	return 1;
};
float VoltageSource::divAtNode(int n, int dn) const{
	return 1;
};