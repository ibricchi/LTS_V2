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

    switch (args.size())
    {
    case 3:
        SetupValues();
        break;
    case 4:
        SetupValues(
            stof(args[3])
        );
        break;
    case 5:
        SetupValues(
            stof(args[3]),
            stof(args[4])
        );
        break;
    case 6:
        SetupValues(
            stof(args[3]),
            stof(args[4]),
            true,
            stof(args[5])
        );
        break;
    default:
        break;
    }
}

void BJT::SetupValues(float _BF, float _IFS, bool _hasVAF, float _VAF){
    BF = _BF;
    AF = BF/(1+BF);
    IFS = _IFS;
    IRS = _IFS;
    hasVAF = _hasVAF;
    VAF = _VAF;
}

float BJT::ivAtNode(int nin) const{
    double VBE = (nodalVoltages[n::B] - nodalVoltages[n::E]);
    double VBC = (nodalVoltages[n::B] - nodalVoltages[n::C]);

    double IBF = (IFS/BF)*(exp(VBE/VT) - 1);
    double IBR = (IRS/BR)*(exp(VBC/VT) - 1);

    double GPF = IFS/BF*exp(VBE/VT)/VT;
    double GPR = IRS/BR*exp(VBC/VT)/VT;

    double GMF = BF*GPF;
    double GMR = BR*GPR;

    double IBFEQ = IBF-GPF*VBE;
    double IBREQ = IBR-GPR*VBC;
    double ICEQ = BF*IBF-BR*IBR;

    double IC = ICEQ + IBREQ + GMF*VBE - GMR*VBC;
    double IB = IBREQ + IBFEQ;
    double IE = IBFEQ + GMF*VBE - GMR*VBC + ICEQ;

    // this is just because I aciddentally set up the switch statement wrong
    // this fixes it, but maybe changing the swtich statement might be more efficient later on
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
};

float BJT::divAtNode(int nin, int dnin) const{
    double VBE = (nodalVoltages[n::B] - nodalVoltages[n::E]);
    double VBC = (nodalVoltages[n::B] - nodalVoltages[n::C]);

    double IBF = (IFS/BF)*(exp(VBE/VT) - 1);
    double IBR = (IRS/BR)*(exp(VBC/VT) - 1);

    double GPF = IFS/BF*exp(VBE/VT)/VT;
    double GPR = IRS/BR*exp(VBC/VT)/VT;

    double GMF = BF*GPF;
    double GMR = BR*GPR;

    double GO = 0; // implement later only for early voltage

    double IBFEQ = IBF-GPF*VBE;
    double IBREQ = IBR-GPR*VBC;
    double ICEQ = BF*IBF-BR*IBR;

    double DIC = ICEQ + IBREQ + GMF*VBE - GMR*VBC;
    double DIB = IBREQ + IBFEQ;
    double DIE = IBFEQ + GMF*VBE - GMR*VBC + ICEQ;

    // this is just because I aciddentally set up the switch statement wrong
    // this fixes it, but maybe changing the swtich statement might be more efficient later on
    int n = nin==nodes[n::C]?n::C:(nin==nodes[n::B]?n::B:n::E);
    int dn = dnin==nodes[n::C]?n::C:(dnin==nodes[n::B]?n::B:n::E);

    double conductance;
    switch(n){
        case n::C:
            switch(dn){
                case n::C:
                    conductance = GPR + GO;
                    break;
                case n::B:
                    conductance = -GPR;
                    break;
                case n::E:
                    conductance = -GO;
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
                    conductance = -GO;
                    break;
                case n::B:
                    conductance = -GPF;
                    break;
                case n::E:
                    conductance = GO + GPF;
                    break;
            }
            break;
    }

    // cout << "n: " << n << " dn: " << dn << " conductance: " << conductance << endl << endl;
    return conductance;
};

vector<int> BJT::getNodes() const{
    return nodes;
}