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
    float VCB = nodalVoltages[n::C] - nodalVoltages[n::B];
    float VCE = nodalVoltages[n::C] - nodalVoltages[n::E];
    float IC, IE;
    if(VBE < 0 && VCB > 0){
        return 0;
    }else if(VBE > 0 && VCB > 0){
        IC = IS*exp(VBE/VT)*(hasVAF?(1 + VCE/VAF):1);
        switch(n)
        {
            case n::C:
                return IC;
                break;
            case n::B:
                return IC/BF;
                break;
            case n::E:
                return IC/AF;
                break;
            default:
                cerr << "This node is not included in this BJT";
                exit(1);
                break;
        }
    }else if(VBE > 0 && VCB < 0){
        switch(n){
            case n::C:
                IC = IS*exp(-VCB/VT);
                return IC;
                break;
            case n::B:
                IC = IS*exp(-VCB/VT);
                IE = IS*exp(VBE/VT);
                return IE - IC;
                break;
            case n::E:
                IE = IS*exp(VBE/VT);
                return IE;
                break;
            default:
                cerr << "This node is not included in this BJT";
                exit(1);
                break;
        }
    }else if(VBE > 0 && VCB > 0){
        IE = IS*exp(-VCB/VT)*(hasVAF?(1 - VCE/VAF):1);
        switch(n)
        {
            case n::C:
                return IE/AF;
                break;
            case n::B:
                return IE/BF;
                break;
            case n::E:
                return IE;
                break;
            default:
                cerr << "This node is not included in this BJT";
                exit(1);
                break;
        }
    }
};

float BJT::divAtNode(int n, int dn) const{
    float VBE = nodalVoltages[n::B] - nodalVoltages[n::E];
    float VCB = nodalVoltages[n::C] - nodalVoltages[n::B];
    float VCE = nodalVoltages[n::C] - nodalVoltages[n::E];
    float IC, IE;
    switch(dn){
        case n::C:
            if(VBE < 0 && VCB > 0){
                return 0;
            }else if(VBE > 0 && VCB > 0){
                IC = IS*exp(VBE/VT)*(hasVAF?1/VAF:0);
                switch(n)
                {
                    case n::C:
                        return IC;
                        break;
                    case n::B:
                        return IC/BF;
                        break;
                    case n::E:
                        return IC/AF;
                        break;
                    default:
                        cerr << "This node is not included in this BJT";
                        exit(1);
                        break;
                }
            }else if(VBE > 0 && VCB < 0){
                switch(n){
                    case n::C:
                        IC = -IS/VT*exp(-VCB/VT);
                        return IC;
                        break;
                    case n::B:
                        IC = -IS/VT*exp(-VCB/VT);
                        IE = 0;
                        return IE - IC;
                        break;
                    case n::E:
                        IE = 0;
                        return IE;
                        break;
                    default:
                        cerr << "This node is not included in this BJT";
                        exit(1);
                        break;
                }
            }else if(VBE > 0 && VCB > 0){
                IE = -IS/VT*exp(-VCB/VT)*(hasVAF?-1/VAF:1);
                switch(n)
                {
                    case n::C:
                        return IE/AF;
                        break;
                    case n::B:
                        return IE/BF;
                        break;
                    case n::E:
                        return IE;
                        break;
                    default:
                        cerr << "This node is not included in this BJT";
                        exit(1);
                        break;
                }
            }
            break;
        case n::B:
            if(VBE < 0 && VCB > 0){
                return 0;
            }else if(VBE > 0 && VCB > 0){
                IC = IS/VT*exp(VBE/VT)*(hasVAF?(1 + VCE/VAF):1);
                switch(n)
                {
                    case n::C:
                        return IC;
                        break;
                    case n::B:
                        return IC/BF;
                        break;
                    case n::E:
                        return IC/AF;
                        break;
                    default:
                        cerr << "This node is not included in this BJT";
                        exit(1);
                        break;
                }
            }else if(VBE > 0 && VCB < 0){
                switch(n){
                    case n::C:
                        IC = IS/VT*exp(-VCB/VT);
                        return IC;
                        break;
                    case n::B:
                        IC = IS/VT*exp(-VCB/VT);
                        IE = IS/VT*exp(VBE/VT);
                        return IE - IC;
                        break;
                    case n::E:
                        IE = IS/VT*exp(VBE/VT);
                        return IE;
                        break;
                    default:
                        cerr << "This node is not included in this BJT";
                        exit(1);
                        break;
                }
            }else if(VBE > 0 && VCB > 0){
                IE = IS/VT*exp(-VCB/VT)*(hasVAF?(1 - VCE/VAF):1);
                switch(n)
                {
                    case n::C:
                        return IE/AF;
                        break;
                    case n::B:
                        return IE/BF;
                        break;
                    case n::E:
                        return IE;
                        break;
                    default:
                        cerr << "This node is not included in this BJT";
                        exit(1);
                        break;
                }
            }
            break;
        case n::E:
            if(VBE < 0 && VCB > 0){
                return 0;
            }else if(VBE > 0 && VCB > 0){
                IC = -IS/VT*exp(VBE/VT)*(hasVAF?-1/VAF:1);
                switch(n)
                {
                    case n::C:
                        return IC;
                        break;
                    case n::B:
                        return IC/BF;
                        break;
                    case n::E:
                        return IC/AF;
                        break;
                    default:
                        cerr << "This node is not included in this BJT";
                        exit(1);
                        break;
                }
            }else if(VBE > 0 && VCB < 0){
                switch(n){
                    case n::C:
                        IC = 0;
                        return IC;
                        break;
                    case n::B:
                        IC = 0;
                        IE = -IS/VT*exp(VBE/VT);
                        return IE - IC;
                        break;
                    case n::E:
                        IE = -IS/VT*exp(VBE/VT);
                        return IE;
                        break;
                    default:
                        cerr << "This node is not included in this BJT";
                        exit(1);
                        break;
                }
            }else if(VBE > 0 && VCB > 0){
                IE = IS*exp(-VCB/VT)*(hasVAF?1/VAF:0);
                switch(n)
                {
                    case n::C:
                        return IE/AF;
                        break;
                    case n::B:
                        return IE/BF;
                        break;
                    case n::E:
                        return IE;
                        break;
                    default:
                        cerr << "This node is not included in this BJT";
                        exit(1);
                        break;
                }
            }
            break;
    }
};

vector<int> BJT::getNodes() const{
    return nodes;
}