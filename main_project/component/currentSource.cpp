#include <string>
#include <vector>
#include <iostream>

#include "currentSource.hpp"

CurrentSource::CurrentSource(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    nodes = processNodes({args[0], args[1]});

    setupBasic();

    if(args.size() == 3){
        float val = getValue(args[2]);
        setupDC(val);
    }else{
        string flow = args[2];
        if(flow == "DC" || flow == "dc" || flow == "Dc" || flow == "dC"){
            setupDC(
                getValue(args[3]) // current
            );
        }else if(flow.size() > 4){ //checks if "flow" is long enough to be SIN(* where * is any character
            currentWaveform.setupWaveform(this, args, extraInfo);
            
            //initialize current value to current at start time (extraInfo[1] = startTime)
            updateVals(extraInfo[1]);
            
            types.push_back(componentType::timeUpdatable);
        }else{
            std::cerr << "Invalid netlist: The syntax of current source is incorrect." <<std::endl;
            exit(1);
        }
    }
}

void CurrentSource::setupBasic(){

	nodalVoltages = {0,0};

    types.push_back(componentType::currentSource);
	types.push_back(componentType::nonVoltageSource);
}

void CurrentSource::setupDC(float _current){
    current = _current;
}

float CurrentSource::getCurrent() const{
    return current;
}

void CurrentSource::updateVals(float time){
    current = currentWaveform.updateVals(time);
}

// iv functions
double CurrentSource::ivAtNode(int n1){
	return current * (n1==nodes[0]?-1:1);
}
double CurrentSource::divAtNode(int n, int dn){
	return 0;
}

string CurrentSource::getTotalCurrentString(const VectorXd &x, int highestNodeNumber,bool overwrite, float voltage, int order){
    return to_string(current);
}

vector<float> CurrentSource::getTimePoints(){
	vector<float> res;	
	for(auto pair: currentWaveform.getTimePointsPairs()){
		res.push_back(pair.first);
	}
	return res;
}
