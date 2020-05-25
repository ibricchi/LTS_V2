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

void BJT::SetupValues(float _BF, float _IS, bool _hasVAF, float _VAF){
    BF = _BF;
    AF = BF/(1+BF);
    IS = _IS;
    IFS = _IS;
    IRS = _IS;
    hasVAF = _hasVAF;
    VAF = _VAF;
}

float BJT::ivAtNode(int nin) const{
    double VBE = (nodalVoltages[n::B] - nodalVoltages[n::E]);
    double VBC = (nodalVoltages[n::B] - nodalVoltages[n::C]);

    double GCC = -IRS/VT*exp(VBC/VT);
    double GCB = AF*IFS/VT*exp(VBE/VT);

    double GBB = AR*IRS/VT*exp(VBC/VT);
    double GBC = -IFS/VT*exp(VBE/VT);

    double IB = -IRS*(exp(VBC/VT)-1) + AF*IFS*(exp(VBE/VT)-1) - GCC*VBC - GCB*VBE;
    double IC = AR*IRS*(exp(VBC/VT)-1) -IFS*(exp(VBE/VT)-1) - GBC*VBC - GBB*VBE;
    double IE = IB + IC;

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

    double GCC = -IRS/VT*exp(VBC/VT);
    double GCB = AF*IFS/VT*exp(VBE/VT);
    double GCE = GCC + GCB;

    double GBC = -IFS/VT*exp(VBE/VT);
    double GBB = AR*IRS/VT*exp(VBC/VT);
    double GBE = GBB + GBC;

    double GEC = GCC + GBC;
    double GEB = GCB + GBB;
    double GEE = GCC + GCB + GBC + GBB;

    // this is just because I aciddentally set up the switch statement wrong
    // this fixes it, but maybe changing the swtich statement might be more efficient later on
    int n = nin==nodes[n::C]?n::C:(nin==nodes[n::B]?n::B:n::E);
    int dn = dnin==nodes[n::C]?n::C:(dnin==nodes[n::B]?n::B:n::E);

    double conductance;
    switch(n){
        case n::C:
            switch(dn){
                case n::C:
                    conductance = -GCC;
                    break;
                case n::B:
                    conductance = -GCB;
                    break;
                case n::E:
                    conductance = GCE;
                    break;
            }
            break;
        case n::B:
            switch(dn){
                case n::C:
                    conductance = -GBC;
                    break;
                case n::B:
                    conductance = -GBB;
                    break;
                case n::E:
                    conductance = GBE;
                    break;
            }
            break;
        case n::E:
            switch(dn){
                case n::C:
                    conductance = GEC;
                    break;
                case n::B:
                    conductance = GEB;
                    break;
                case n::E:
                    conductance = -GEE;
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