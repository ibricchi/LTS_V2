#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "bjt.hpp"

using namespace std;

BJT::BJT(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    modelName = (args.size()>3) ? args[3] : "";

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
    VCE = // temporary

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
            lastIc = -current;
            break;
        case n::B:
            current = IB;
            lastIb = -current;
            break;
        case n::E:
            current = -IE;
            lastIe = -current;
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
                    conductance = (NPN?GPR:GPF) + GO + GMR;
                    break;
                case n::B:
                    conductance = -(NPN?GPR:GPF) + GMF - GMR;
                    break;
                case n::E:
                    conductance = -GO - GMF;
                    break;
            }
            break;
        case n::B:
            switch(dn){
                case n::C:
                    conductance = -(NPN?GPR:GPF);
                    break;
                case n::B:
                    conductance = GPR + GPF;
                    break;
                case n::E:
                    conductance = -(NPN?GPF:GPR);
                    break;
            }
            break;
        case n::E:
            switch(dn){
                case n::C:
                    conductance = -GO - GMR;
                    break;
                case n::B:
                    conductance = -(NPN?GPF:GPR) + GMR - GMF;
                    break;
                case n::E:
                    conductance = (NPN?GPF:GPR) + GO + GMF;
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

    //Currently only added currents through the current sources!
    //Add the remaining ones once we know that this BJT model produces correct results/converges at all.
    return to_string(lastIc) + "," + to_string(lastIb) + "," + to_string(lastIe);
}