#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "bjt.hpp"

using namespace std;

BJT::BJT(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    // figures out if 3 or 4 inputs are used for BJT and then set's model name appropriately
    if(args.size()==3){
        modelName = "";
    }else if(args.size()==4){
        if(args[3][0] > '9'){
            modelName = args[3];
        }else{
            modelName = "";
        }
    }else if(args.size()==5){
        modelName = args[4];
    }else{
        cerr << "Wrong number of argumnets passed to BJT" << endl;
        exit(1);
    }
    
    // Order: C, B, E
    nodes = processNodes({args[n::C], args[n::B], args[n::E]});

    setNodalVoltages({0,0,0});

	types.push_back(componentType::nonVoltageSource);
	types.push_back(componentType::nonLinear);
}

void BJT::addParam(int paramId, float paramValue){
    switch(static_cast<bjtParamType>(paramId)){ //need this as strongly typed enums don't automatically convert to their underlying type
        case bjtParamType::TYPE:
            NPN = false;
            break;
        case bjtParamType::BF:
            BF = paramValue;
            break;
        case bjtParamType::VAF:
            hasVAF = true;
            VAF = paramValue;
            break;
        case bjtParamType::IFS:
            IFS = paramValue;
            break;
        case bjtParamType::BR:
            BR = paramValue;
            break;
        case bjtParamType::VT:
            VT = paramValue;
            break;
        case bjtParamType::IRS:
            IRS = paramValue;
            break;
        default:
            cerr << "unsupported parameter ID for BJT" << endl;
            exit(1);
            break;
    }
}

void BJT::setNodalVoltages(vector<float> v){
    nodalVoltages = v;
    VBE = (nodalVoltages[n::B] - nodalVoltages[n::E]) * (NPN?1:-1);
    VBC = (nodalVoltages[n::B] - nodalVoltages[n::C]) * (NPN?1:-1);
    VCE = (nodalVoltages[n::C] - nodalVoltages[n::E]) * (NPN?1:-1);

    IBF = (IFS/BF)*(exp(VBE/VT) - 1);
    IBR = (IRS/BR)*(exp(VBC/VT) - 1);
    IC1 = BF*IBF-BR*IBR;

    GPF = IFS/BF*exp(VBE/VT)/VT;
    GPR = IRS/BR*exp(VBC/VT)/VT;

    GMF = BF*GPF;
    GMR = BR*GPR;
    GO = 0; //temporary

    IBFEQ = IBF - GPF*VBE;
    IBREQ = IBR - GPR*VBC;
    ICEQ = IC1 - GMF*VBE + GMR*VBC - GO*VCE;

    //total current produced by currents sources at a node
    IC = (NPN?(ICEQ - IBREQ):(-ICEQ+IBFEQ)); // always current flowing into C
    IB = (IBREQ + IBFEQ) * (NPN?1:-1); // always current flowing into B
    IE = (NPN?(IBFEQ + ICEQ):(-IBREQ-ICEQ)); // always current flowing out of E
}

double BJT::ivAtNode(int nin){
    // selects the right node type (Collector Base or Emitter)
    int n = nin==nodes[n::C]?n::C:(nin==nodes[n::B]?n::B:n::E);

    double current;
    switch(n){
        case n::C:
            current = IC;
            break;
        case n::B:
            current = IB;
            break;
        case n::E:
            current = -IE;
            break;
    }
    // cout << "n: " << n << " current: " << current << endl << endl;
    return current;
}

double BJT::divAtNode(int nin, int dnin){
    // selects the right node type (Collector Base or Emitter)
    int n = nin==nodes[n::C]?n::C:(nin==nodes[n::B]?n::B:n::E);
    int dn = dnin==nodes[n::C]?n::C:(dnin==nodes[n::B]?n::B:n::E);

    double conductance;
    switch(n){
        case n::C:
            switch(dn){
                case n::C:
                    conductance = GPR + GO + GMR;
                    break;
                case n::B:
                    conductance = -GPR + GMF - GMR;
                    break;
                case n::E:
                    conductance = -GO - GMF;
                    break;
            }
            break;
        case n::B:
            switch(dn){
                case n::C:
                    conductance = -GPR;
                    break;
                case n::B:
                    conductance = GPR + GPF;
                    break;
                case n::E:
                    conductance = -GPF;
                    break;
            }
            break;
        case n::E:
            switch(dn){
                case n::C:
                    conductance = -GO - GMR;
                    break;
                case n::B:
                    conductance = -GPF + GMR - GMF;
                    break;
                case n::E:
                    conductance = GPF + GO + GMF;
                    break;
            }
            break;
    }

    // cout << "n: " << n << " dn: " << dn << " conductance: " << conductance << endl << endl;
    return conductance;
}

string BJT::getModelName() const{
    return modelName;
}

string BJT::getCurrentHeadingName() const{
    return "ic_" + name + ",ib_" + name + ",ie_" + name;
}

string BJT::getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    // current through current source, current through resistors, current through dependent current sources
    if(NPN){
        return to_string(IC + GO*VCE - GMR*VBC + GMF*VBE - GPR*VBC) + "," + to_string(IB + GPF*VBE + GPR*VBC) + "," + to_string(-IE - GMF*VBE + GMR*VBC - GPF*VBE - GO*VCE);
    }else{
        return to_string(IC - GO*VCE + GMR*VBC - GMF*VBE + GPF*VBC) + "," + to_string(IB - GPF*VBE - GPR*VBC) + "," + to_string(-IE + GMF*VBE - GMR*VBC + GPR*VBE + GO*VCE);
    }
}