#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "mosfet.hpp"

using namespace std;

Mosfet::Mosfet(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    // Order: C, B, E
    nodes = processNodes({args[n::D], args[n::G], args[n::S]});

    nodalVoltages = {0,0,0};

	types.push_back(componentType::nonVoltageSource);
	types.push_back(componentType::nonLinear);
}

void Mosfet::addParam(int paramId, float paramValue){
    switch(static_cast<mosfetParamType>(paramId)){ //need this as strongly typed enums don't automatically convert to their underlying type
        case mosfetParamType::ISat:
            ISat = paramValue;
            break;
        case mosfetParamType::K:
            K = paramValue;
            break;
        case mosfetParamType::VA:
            VA = paramValue;
            break;
        case mosfetParamType::VT:
            VT = paramValue;
            break;
    }
}

float Mosfet::ivAtNode(int nin) const{
    double VGS = (nodalVoltages[n::G] - nodalVoltages[n::S]);
    double VDS = (nodalVoltages[n::D] - nodalVoltages[n::S]);

    float IS, GM, GO;

    if(VGS - VT < 0){
        IS = 0;
        GM = 0;
        GO = 0;
    }else if(VGS-VT < VDS){
        IS = K/2*(VGS-VT)*();
        GM = sqrt(2*K*IS);
        GO = hasVA?IS/VA:0;
    }else if(-VSD <= -VSG-VT){
        IS = K*((VSG-VT)-VSD/2)*VSD;
        GM = K*VSD;
        GO = K*((VSG-VT)-VSD);
    }else{
        cerr << "mosfet in a non supported state" << endl;
        exit(1);
    }

    // this is just because I aciddentally set up the switch statement wrong
    // this fixes it, but maybe changing the swtich statement might be more efficient later on
    int n = nin==nodes[n::D]?n::D:(nin==nodes[n::G]?n::G:n::S);

    double current;
    switch(n){
        case n::D:
            current = IS-GM*VSG;
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

float Mosfet::divAtNode(int nin, int dnin) const{
    double VSG = (nodalVoltages[n::S] - nodalVoltages[n::G]);
    double VSD = (nodalVoltages[n::S] - nodalVoltages[n::D]);

    float IS, GM, GO;

    if(VSG - VT < 0){
        IS = 0;
        GM = 0;
        GO = 0;
    }else if(-VSG-VT < -VSD){
        IS = K/2*(VSG-VT)*(VSG-VT);
        GM = sqrt(2*K*IS);
        GO = hasVA?IS/VA:0;
    }else if(-VSD <= -VSG-VT){
        IS = K*((VSG-VT)-VSD/2)*VSD;
        GM = K*VSD;
        GO = K*((VSG-VT)-VSD);
    }else{
        cerr << "mosfet in a non supported state" << endl;
        exit(1);
    }

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

vector<int> Mosfet::getNodes() const{
    return nodes;
}