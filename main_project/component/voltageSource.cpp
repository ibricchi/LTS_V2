#include <string>
#include <iostream>

#include "voltageSource.hpp"

VoltageSource::VoltageSource(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    nodes = processNodes({args[0], args[1]});

    voltageSourcesIdx = extraInfo[2];

    setupBasic();

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

void VoltageSource::setupBasic(){

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
double VoltageSource::ivAtNode(int n){
	return 1;
}
double VoltageSource::divAtNode(int n, int dn){
	return 1;
}

string VoltageSource::getTotalCurrentString(const VectorXd &x, int highestNodeNumber, bool overwrite, float voltage, int order) {
    return to_string(x(highestNodeNumber+voltageSourcesIdx));
}

vector<float> VoltageSource::getTimePoints(){
	vector<float> res;	
	for(auto pair: voltageWaveform.getTimePointsPairs()){
		res.push_back(pair.first);
	}
	return res;
}
