#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "bjt.hpp"

using namespace std;

BJT::BJT(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    int NC = stoi(args[n::C]);
    int NB = stoi(args[n::B]);
    int NE = stoi(args[n::E]);

    nodalVoltages = {0,0};

    nodes.push_back(NC);
    nodes.push_back(NB);
    nodes.push_back(NE);

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
    hasVAF = _hasVAF;
    VAF = _VAF;
}

float BJT::ivAtNode(int nin) const{
    double VBE = (nodalVoltages[n::B] - nodalVoltages[n::E]);
    double VBC = (nodalVoltages[n::B] - nodalVoltages[n::C]);
    double VCE = (nodalVoltages[n::C] - nodalVoltages[n::E]);

    // this is just because I aciddentally set up the switch statement wrong
    // this fixes it, but maybe changing the swtich statement might be more efficient later on
    int n = nin==nodes[n::C]?n::C:(nin==nodes[n::B]?n::B:n::E);

    double current;
    switch(n){
        case n::C:
            current = IS*(exp(VBE/VT) - exp(VBC/VT)*(1+1/BR) + 1/BR);
            break;
        case n::B:
            current = IS*(1/BF*(exp(VBE/VT)-1) + 1/BR*(exp(VBC/VT)-1));
            break;
        case n::E:
            current = -IS*(-exp(VBC/VT) + exp(VBE/VT)*(1+1/BF) - 1/BF);
            break;
    }
    // cout << "n: " << n << " current: " << current << endl << endl;
    return current;
};

float BJT::divAtNode(int nin, int dnin) const{
    double VBE = (nodalVoltages[n::B] - nodalVoltages[n::E]);
    double VBC = (nodalVoltages[n::B] - nodalVoltages[n::C]);
    double VCE = (nodalVoltages[n::C] - nodalVoltages[n::E]);

    // this is just because I aciddentally set up the switch statement wrong
    // this fixes it, but maybe changing the swtich statement might be more efficient later on
    int n = nin==nodes[n::C]?n::C:(nin==nodes[n::B]?n::B:n::E);
    int dn = dnin==nodes[n::C]?n::C:(dnin==nodes[n::B]?n::B:n::E);

    double conductance;
    switch(n){
        case n::C:
            switch(dn){
                case n::C:
                    conductance = IS/VT*exp(VBC/VT)*(1+1/BR);
                    break;
                case n::B:
                    conductance = IS/VT*(exp(VBE/VT) - exp(VBC/VT)*(1+1/BR));
                    break;
                case n::E:
                    conductance = IS/VT*exp(VBE/VT);
                    break;
            }
            break;
        case n::B:
            switch(dn){
                case n::C:
                    conductance = -IS/VT/BR*exp(VBC/VT);
                    break;
                case n::B:
                    conductance = IS/VT*(exp(VBE/VT)/BF + exp(VBC/VT)/BR);
                    break;
                case n::E:
                    conductance = IS/VT/BF*exp(VBE/VT);
                    break;
            }
            break;
        case n::E:
            switch(dn){
                case n::C:
                    conductance = IS/VT*exp(VBC/VT);
                    break;
                case n::B:
                    conductance = IS/VT*(-exp(VBC/VT) + exp(VBE/VT)*(1+1/BF));
                    break;
                case n::E:
                    conductance = IS/VT*exp(VBE/VT)*(1+1/BF);
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