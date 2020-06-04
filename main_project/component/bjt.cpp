#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "bjt.hpp"

using namespace std;

BJT::BJT(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    // Order: C, B, E
    nodes = processNodes({args[n::C], args[n::B], args[n::E]});

    nodalVoltages = {0,0,0};

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

double BJT::ivAtNode(int nin){
    double VBE = (nodalVoltages[n::B] - nodalVoltages[n::E]);
    double VBC = (nodalVoltages[n::B] - nodalVoltages[n::C]);
    double VCE = 0; //temporary

    double IBF = (IFS/BF)*(exp(VBE/VT) - 1);
    double IBR = (IRS/BR)*(exp(VBC/VT) - 1);
    double IC1 = BF*IBF-BR*IBR;

    double GPF = IFS/BF*exp(VBE/VT)/VT;
    double GPR = IRS/BR*exp(VBC/VT)/VT;

    double GMF = BF*GPF;
    double GMR = BR*GPR;
    double GO = 0; //temporary

    double IBFEQ = IBF - GPF*VBE;
    double IBREQ = IBR - GPR*VBC;
    double ICEQ = IC1 - GMF*VBE + GMR*VBC - GO*VCE;

    double IC = ICEQ - IBREQ;
    double IB = IBREQ + IBFEQ;
    double IE = IBFEQ + ICEQ;

    // this is just because I aciddentally set up the switch statement wrong
    // this fixes it, but maybe changing the swtich statement might be more efficient later on
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
    double VBE = (nodalVoltages[n::B] - nodalVoltages[n::E]);
    double VBC = (nodalVoltages[n::B] - nodalVoltages[n::C]);

    double IBF = (IFS/BF)*(exp(VBE/VT) - 1);
    double IBR = (IRS/BR)*(exp(VBC/VT) - 1);

    double GPF = IFS/BF*exp(VBE/VT)/VT;
    double GPR = IRS/BR*exp(VBC/VT)/VT;

    double GMF = BF*GPF;
    double GMR = BR*GPR;

    double GO = 0; // implement later only for early voltage

    // this is just because I aciddentally set up the switch statement wrong
    // this fixes it, but maybe changing the swtich statement might be more efficient later on
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
                    conductance = GO + GPF + GMF;
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