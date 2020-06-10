#include <iostream>

#include "component.hpp"

using namespace std;

// basic default constructor for a component takes a name in
Component::Component(string _name)
    :name{_name}{};
Component::~Component(){}

// basic default functions for all components that are not overloaded
string Component::getName() const{
    return name;
}
vector<int> Component::getNodes() const{
	return nodes;
}
vector<componentType> Component::getTypes() const{
	return types;
}
void Component::appendToNodes(int nodeToAppend){
	nodes.push_back(nodeToAppend);
}

// default current header name, overloaded for more complex multi-node components
string Component::getCurrentHeadingName() const{
	return "i_" + name;
}
// default nodal voltage setter, overloading is done in certain cases to increase efficiency
void Component::setNodalVoltages(vector<float> v){
    nodalVoltages = v;
}


// these functions should by default throw error unless overwritten by another child class down the line
// this allows to call an error if ever a function that shouldn't be returning a voltage or current or conductance
// is asked for one
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
vector<float> Component::getTimePoints(){
	vector<float> n;
	return n;
}
void Component::setTimePoints(){
	return;
}

// Get Value function is used to obtain the actual value of a function with units
// Multiplies by the magniutde of the unit and rmeoves the speicifc unit text
float Component::getValue(string val){
	string units{};
	float base{};//base number
	float mult{};//what we multiply the number by
	int ss = -1;//string start -1 by default

	// for loop to find where the numbers end and the string part starts
	for(int i = 0; i < val.size(); i++){
		// all letter ascii characters are greater than number characters
		if(val[i] > '9'){
			ss = i;
			break;
		}
	}
	
	// checks if any string characters were found
	if(ss != -1){
		base = stof(val.substr(0,ss)); // converts the number part of the input into a float
		units = val.substr(ss, val.size() - ss); // separates the string part from the number part
		char u = units[0]; // first character of the units is normally the multiplier
		switch(u){
			case 'F': case 'f': // fento
				mult = 1e-15;
				break;
			case 'P': case 'p': // pico
				mult = 1e-12;
				break;
			case 'N': case 'n': // nano
				mult = 1e-9;
				break;
			case 'U': case 'u': // micro
				mult = 1e-6;
				break;
			case 'K': case 'k': // kilo
				mult = 1e3;
				break;
			case 'X': case 'x': // mega
				mult = 1e6;
				break;
			case 'G': case 'g': // giga
				mult = 1e9;
				break;
			case 'T': case 't': // tera
				mult = 1e12;
				break;
			case 'M': case 'm': // m can go into several directions
				if(units.size() > 2){
					if((units[1] == 'E' && units[2] == 'G') || (units[1] == 'e' && units[2] == 'g')){ // mega
						mult = 1e6;
						break;
					}else if((units[1] == 'I' && units[2] == 'L') || (units[1] == 'i' && units[2] == 'l')){ // mil-inch
						mult = 25.4e-6;
						break;
					}
				}
				mult = 1e-3; // if only 1 m must be milli
				break;
			default:
				mult = 1; // default is 1
				break;
		}
		return base * mult; // does the conversion
	}else{
		return stof(val); // if no units then returns the numbers
	}
}

// helper function to convert nodes written in the form NXXX into a numeric form
// used in consturction of all components
vector<int> Component::processNodes(vector<string> rawNodes){
    vector<int> nodeNumbers{};
	// loops trhough all nodes present in the component
    for(auto& node : rawNodes){
        if(node == "0"){ // checks if node si a ground node
            nodeNumbers.push_back(0);
        }else if(node[0] == 'n' || node[0] == 'N'){ // checks if starts with an n
			if(stoi(node.substr(1)) == 0){ // ground nodes cannot start with an n
				cerr << "A zero (ground) node cannot start with N" <<endl;
				exit(1);
			}
            nodeNumbers.push_back(stoi(node.substr(1))); //remove leading N
        }else{
            cerr << "A specified node is invalid. A node must either be 0 or have the format N123" <<endl;
            exit(1);
        }
    }
    return nodeNumbers; // returns converted nodes
}