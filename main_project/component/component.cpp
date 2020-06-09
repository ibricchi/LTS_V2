#include <iostream>

#include "component.hpp"

using namespace std;

Component::Component(string _name)
    :name{_name}{};
Component::~Component(){}

// these functions should by default throw error unless overwritten by another child class down the line
// this allows to call an error if ever a function that shouldn't be returning a voltage or current or conductance
// is asked for one

string Component::getName() const{
    return name;
}

string Component::getCurrentHeadingName() const{
	return "i_" + name;
}

vector<int> Component::getNodes() const{
	return nodes;
}

float Component::getVoltage() const{
	cerr << "Tried to call getVoltage from a component that doesn't support it" <<endl;
	exit(1);
}

float Component::getCurrent() const{
   	cerr << "Tried to call getCurrent from a component that doesn't support it" <<endl;
	exit(1);
}

float Component::getConductance() const{
    cerr << "Tried to call getConductance from a component that doesn't support it" <<endl;
	exit(1);
}

float Component::getGain() const{
	cerr << "Tried to call getGain from a component that doesn't support it" <<endl;
	exit(1);
}

string Component::getVsName() const{
	cerr << "Tried to call getVSName from a component that doesn't support it" <<endl;
	exit(1);
}

void Component::updateVals(float newVoltage, float newCurrent, int order){
    cerr << "Tried to call updateVals from a component that doesn't support it" <<endl;
	exit(1);
}

void Component::updateVals(float time_or_voltage){
    cerr << "Tried to call updateVals from a component that doesn't support it" <<endl;
	exit(1);
}

void Component::addParam(int paramId, float paramValue){
	cerr << "Tried to call addParam from a component that doesn't support it" <<endl;
	exit(1);
}

void Component::setTimeStep(double _timeStep){
	cerr << "Tried to call setTimeStep from a component that doesn't support it" <<endl;
	exit(1);
}

string Component::getModelName() const{
	cerr << "Tried to call getModelName from a component that doesn't support it" <<endl;
	exit(1);
}

void Component::initCompCurrent(float _voltage_or_current){
	cerr << "Tried to call initCompCurrent from a component that doesn't support it" << endl;
	exit(1);
}

void Component::setMinPNConductance(float con){
	cerr << "Tried to call setMinPNConductance from a component that doesn't support it" << endl;
	exit(1);
}

float Component::getValue(string val){
	string units{};
	float base{};//base number
	float mult{};//what we multiply the number by
	int ss = -1;//string start
	for(int i = 0; i < val.size(); i++){
		if(val[i] > '9'){
			ss = i;
			break;
		}
	}
	if(ss != -1){
		base = stof(val.substr(0,ss));
		units = val.substr(ss, val.size() - ss);
		char u = units[0];
		switch(u){
			case 'F': case 'f':
				mult = 1e-15;
				break;
			case 'P': case 'p':
				mult = 1e-12;
				break;
			case 'N': case 'n':
				mult = 1e-9;
				break;
			case 'U': case 'u':
				mult = 1e-6;
				break;
			case 'K': case 'k':
				mult = 1e3;
				break;
			case 'X': case 'x':
				mult = 1e6;
				break;
			case 'G': case 'g':
				mult = 1e9;
				break;
			case 'T': case 't':
				mult = 1e12;
				break;
			case 'M': case 'm':
				if(units.size() > 2){
					if((units[1] == 'E' && units[2] == 'G') || (units[1] == 'e' && units[2] == 'g')){
						mult = 1e6;
						break;
					}else if((units[1] == 'I' && units[2] == 'L') || (units[1] == 'i' && units[2] == 'l')){
						mult = 25.4e-6;
						break;
					}
				}
				mult = 1e-3;
				break;
			default:
				mult = 1;
				break;
		}
		return base * mult;
	}else{
		return stof(val);
	}
}

vector<int> Component::processNodes(vector<string> rawNodes){
    vector<int> nodeNumbers{};
    for(auto& node : rawNodes){
        if(node == "0"){
            nodeNumbers.push_back(0);
        }else if(node[0] == 'n' || node[0] == 'N'){
			if(stoi(node.substr(1)) == 0){
				cerr << "A zero (ground) node cannot start with N" <<endl;
				exit(1);
			}
            nodeNumbers.push_back(stoi(node.substr(1))); //remove leading N
        }else{
            cerr << "A specified node is invalid. A node must either be 0 or have the format N123" <<endl;
            exit(1);
        }
    }
    return nodeNumbers;
}

vector<componentType> Component::getTypes() const{
	return types;
}

// functions for non linear
// set functions
void Component::setNodalVoltages(vector<float> v){
    nodalVoltages = v;
}

void Component::appendToNodes(int nodeToAppend){
	nodes.push_back(nodeToAppend);
}
