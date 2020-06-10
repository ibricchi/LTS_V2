#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "bjt.hpp"

using namespace std;

// main standard constructor for BJT using name args and extra info parameters
BJT::BJT(string name, vector<string> args, vector<float> extraInfo)
    :Component{name} // constructs parent class with name
{
    // figures out if 3 or 4 inputs are used for BJT and then set's model name appropriately
    if(args.size()==3){ // if only 3 args are passed then no model name can be present
        modelName = "";
    }else if(args.size()==4){
        if(args[3][0] > '9'){ // checks if first character of 4th arg is a number, if not must be model name
            modelName = args[3];
        }else{ // if first char of 4th arg is a number must be 4th node
            modelName = "";
        }
    }else if(args.size()==5){ // if 5 args passed then 5th arg must be model name
        modelName = args[4];
    }else{ // any other number of args is an error
        cerr << "Wrong number of argumnets passed to BJT" << endl;
        exit(1);
    }
    
    // get's minimum pn conductnace from extrainfo
    minPNConductance = extraInfo[3];

    // Order of nodes: C, B, E
    // 4th node always ignroed since we don't actually implement it's effect
    nodes = processNodes({args[n::C], args[n::B], args[n::E]});

    // defaults all node and model parameters
    setNodalVoltages({0,0,0});

    // sets up component types
	types.push_back(componentType::nonVoltageSource); // allows the use of IV and DIV functions to be called
	types.push_back(componentType::nonLinear); // causes the use of non-lineaer analysis
}

// set's up use of model with standard required model function
string BJT::getModelName() const{
    return modelName;
}
void BJT::addParam(int paramId, float paramValue){
    switch(static_cast<bjtParamType>(paramId)){ //need this as strongly typed enums don't automatically convert to their underlying type
        case bjtParamType::TYPE: // type is defined by the type of model PNP or NPN
            NPN = false;
            break;
        case bjtParamType::BF: // sets forward beta
            BF = paramValue;
            break;
        case bjtParamType::VAF: // sets Early Voltage
            hasVAF = true;  // turns early votlage calculations on
            VAF = paramValue;
            break;
        case bjtParamType::IFS: // sets up forward saturation current
            IFS = paramValue;
            break;
        case bjtParamType::BR: // sets up reverse beta
            BR = paramValue;
            break;
        case bjtParamType::VT: // sets up thermal voltage
            VT = paramValue;
            break;
        case bjtParamType::IRS: // sets up reveerse saturation current
            IRS = paramValue;
            break;
        default: // checks if unknown parameter is attempted to be set
            cerr << "unsupported parameter ID for BJT" << endl;
            exit(1);
            break;
    }
}

// overload of setNodalVoltages to only calculate model component parameters when voltages are updated
void BJT::setNodalVoltages(vector<float> v){
    nodalVoltages = v; // basic votlage setup
    VBE = (nodalVoltages[n::B] - nodalVoltages[n::E]) * (NPN?1:-1); // Voltage between base and emitter
    VBC = (nodalVoltages[n::B] - nodalVoltages[n::C]) * (NPN?1:-1); // votlage between base and collector
    VCE = (nodalVoltages[n::C] - nodalVoltages[n::E]) * (NPN?1:-1); // votlage between collector and emitter

    IBF = (IFS/BF)*(exp(VBE/VT) - 1); // current IBF
    IBR = (IRS/BR)*(exp(VBC/VT) - 1); // current IBR
    IC1 = BF*IBF-BR*IBR; // Collector current

    // sets up resistors
    GPF = IFS/BF*exp(VBE/VT)/VT;
    GPR = IRS/BR*exp(VBC/VT)/VT;
    GO = (hasVAF?(IC1 + IBF)/VAF:0); // sets up early votlage resistor if early voltage is being modeled

    // ensures all resistor conductances are aboce the minimum PN juntion conductance
    if(GPF < minPNConductance) GPF = minPNConductance;
    if(GPR < minPNConductance) GPR = minPNConductance;
    if(GO < minPNConductance) GO = minPNConductance;

    // sets up dependant current sources
    GMF = BF*GPF;
    GMR = BR*GPR;

    // calculate current sources
    IBFEQ = IBF - GPF*VBE;
    IBREQ = IBR - GPR*VBC;
    ICEQ = IC1 - GMF*VBE + GMR*VBC - GO*VCE;

    //total current produced by currents sources at a node
    IC = (NPN?(ICEQ - IBREQ):(-ICEQ+IBFEQ)); // always current flowing into C
    IB = (IBREQ + IBFEQ) * (NPN?1:-1); // always current flowing into B
    IE = (NPN?(IBFEQ + ICEQ):(-IBREQ-ICEQ)); // always current flowing out of E
}

double BJT::ivAtNode(int nin){
    // selects the right node type (Collector Base or Emitter) by checking order in nodes vector
    int n = nin==nodes[n::C]?n::C:(nin==nodes[n::B]?n::B:n::E);

    // these exlude dependant current sources which are modeled as conductances
    double current;
    switch(n){
        case n::C: // current sources at collector
            current = IC;
            break;
        case n::B: // currnet soruces at base
            current = IB;
            break;
        case n::E: // current sources at emitter
            current = -IE;
            break;
    }
    // cout << "n: " << n << " current: " << current << endl << endl;
    return current;
}

double BJT::divAtNode(int nin, int dnin){
    // selects the right node type (Collector Base or Emitter) by checking order in nodes vector
    int n = nin==nodes[n::C]?n::C:(nin==nodes[n::B]?n::B:n::E);
    int dn = dnin==nodes[n::C]?n::C:(dnin==nodes[n::B]?n::B:n::E);

    // models conducances between two nodes, this includes voltage dependant current sources
    double conductance;
    switch(n){
        case n::C:
            switch(dn){
                case n::C: // conductance looking at C based on voltage at C
                    conductance = GPR + GO + GMR;
                    break;
                case n::B: // conductance looking at C based on voltage at B
                    conductance = -GPR + GMF - GMR;
                    break;
                case n::E: // conductance looking at C based on voltage at E
                    conductance = -GO - GMF;
                    break;
            }
            break;
        case n::B:
            switch(dn){
                case n::C: // conductance looking at B based on voltage at C
                    conductance = -GPR;
                    break;
                case n::B: // conductance looking at B based on voltage at B
                    conductance = GPR + GPF;
                    break;
                case n::E: // conductance looking at B based on voltage at E
                    conductance = -GPF;
                    break;
            }
            break;
        case n::E:
            switch(dn){
                case n::C: // conductance looking at E based on voltage at C
                    conductance = -GO - GMR;
                    break;
                case n::B: // conductance looking at E based on voltage at B
                    conductance = -GPF + GMR - GMF;
                    break;
                case n::E: // conductance looking at E based on voltage at E
                    conductance = GPF + GO + GMF;
                    break;
            }
            break;
    }

    return conductance;
}

// overloaded to allow use of options command to set min PN conductance
void BJT::setMinPNConductance(float con){
    minPNConductance = con;
}

// overloads current outputting methods to properly repesent all nodes
string BJT::getCurrentHeadingName() const{
    return "ic_" + name + ",ib_" + name + ",ie_" + name;
}
string BJT::getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    // current through current source, current through resistors, current through dependent current sources
    // checks component type
    if(NPN){// calculates collector, then base, then emitter currents separated with commas for CSV files
        return to_string(IC + GO*VCE - GMR*VBC + GMF*VBE - GPR*VBC) + "," + to_string(IB + GPF*VBE + GPR*VBC) + "," + to_string(-IE - GMF*VBE + GMR*VBC - GPF*VBE - GO*VCE);
    }else{
        return to_string(IC - GO*VCE + GMR*VBC - GMF*VBE + GPR*VBC) + "," + to_string(IB - GPF*VBE - GPR*VBC) + "," + to_string(-IE + GMF*VBE - GMR*VBC + GPF*VBE + GO*VCE);
    }
}