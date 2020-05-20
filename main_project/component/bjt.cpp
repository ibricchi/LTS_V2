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
	types.push_back(componentType::nonLinears);

    switch (args.size())
    {
    case 2:
        SetupValues();
        break;
    case 3:
        SetupValues(
            stof(args[2])
        );
        break;
    case 4:
        SetupValues(
            stof(args[2]),
            stof(args[3])
        );
        break;
    case 5:
        SetupValues(
            stof(args[2]),
            stof(args[3]),
            true,
            stof(args[4])
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

float BJT::ivAtNode(int n) const{
    float VBE = nodalVoltages[n::B] - nodalVoltages[n::E];
    float VBC = nodalVoltages[n::B] - nodalVoltages[n::C];
    float VCE = nodalVoltages[n::C] - nodalVoltages[n::E];
    float current;
    switch(n){
        case n::C:
            current = IS*(exp(VBE/VT) - exp(VBC/VT)*(1+1/BR) + 1/BR);
            return current;
            break;
        case n::B:
            current = IS*(1/BF*(exp(VBE/VT)-1)+1/BR*(exp(VBC/VT)-1));
            return current;
            break;
        case n::E:
            current = IS*(-exp(VBC/VT) + exp(VBE/VT)*(1+1/BF) - 1/BF);
            return -current;
            break;
    }
};

float BJT::divAtNode(int n, int dn) const{
    float VBE = nodalVoltages[n::B] - nodalVoltages[n::E];
    float VBC = nodalVoltages[n::B] - nodalVoltages[n::C];
    float VCE = nodalVoltages[n::C] - nodalVoltages[n::E];
    float conductance;
    switch(n){
        case n::C:
            switch(dn){
                case n::C:
                    conductance = IS/VT*exp(VBC/VT)*(1+1/BR);
                    return conductance;
                    break;
                case n::B:
                    conductance = IS*(exp(VBE/VT) - exp(VBE/VT)*(1+1/BR));
                    return conductance;
                    break;
                case n::E:
                    conductance = -IS/VT*exp(VBE/VT);
                    return -conductance;
                    break;
            }
            break;
        case n::B:
            switch(dn){
                case n::C:
                    conductance = -IS/VT/BR*exp(VBC/VT);
                    return conductance;
                    break;
                case n::B:
                    conductance = IS/VT*(exp(VBE/VT)/BF + exp(VBC/VT)/BR);
                    return conductance;
                    break;
                case n::E:
                    conductance = -IS/VT/BF*exp(VBE/VT);
                    return -conductance;
                    break;
            }
            break;
        case n::E:
            switch(dn){
                case n::C:
                    conductance = IS/VT*exp(VBE/VT);
                    return conductance;
                    break;
                case n::B:
                    conductance = IS/VT*(-exp(VBC/VT) + exp(VBE/VT)*(1+1/BF));
                    return conductance;
                    break;
                case n::E:
                    conductance = -IS/VT*exp(VBE/VT)*(1+1/BF);
                    return -conductance;
                    break;
            }
            break;
    }
};

vector<int> BJT::getNodes() const{
    return nodes;
}