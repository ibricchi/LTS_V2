#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "mosfet.hpp"

using namespace std;

Mosfet::Mosfet(string name, vector<string> args, vector<float> extraInfo)
    :Component{name}
{
    modelName = (args.size()>3) ? args[3] : "";
    // Order: C, B, E
    nodes = processNodes({args[n::D], args[n::G], args[n::S]});

    nodalVoltages = {0,0,0};

	types.push_back(componentType::nonVoltageSource);
	types.push_back(componentType::nonLinear);
}

void Mosfet::addParam(int paramId, float paramValue){
    switch(static_cast<mosfetParamType>(paramId)){ //need this as strongly typed enums don't automatically convert to their underlying type
        case mosfetParamType::TYPE:
            NMOS = false;
            break;
        case mosfetParamType::K:
            K = paramValue;
            break;
        case mosfetParamType::VA:
            hasVA=true;
            VA = paramValue;
            break;
        case mosfetParamType::VT:
            VT = paramValue;
            break;
    }
}

string Mosfet::getModelName() const{
    return modelName;
}

double Mosfet::ivAtNode(int nin){
    double VGS = (nodalVoltages[n::G] - nodalVoltages[n::S]) * (NMOS?1:-1);
    double VDS = (nodalVoltages[n::D] - nodalVoltages[n::S]) * (NMOS?1:-1);

    float ID, GM, GO;
    ID = 0;

    if(VGS-VT<0){
        ID = 0;
        GM = 0;
        GO = 0;
    }else if(NMOS?(VGS-VT<VDS):(0<VDS+VGS+VT)){
        ID = K * (VGS-VT)*(VGS-VT) * (hasVA?(1 + VDS/VA):1);
        GM = sqrt(2*K*ID);
        GO = ID/VA;
    }else if(NMOS?(VDS <= VGS-VT):(VDS+VGS+VT<=0)){
        ID = K * (2*(VGS-VT)*VDS-VDS*VDS);
        GM = K*VDS;
        GO = K*((VGS-VT)-VDS);
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
            current = ID-GM*VGS-GO*VDS;
            lastId = -current;
            break;
        case n::G:
            current = 0;
            lastIg = -current;
            break;
        case n::S:
            current = -(ID-GM*VGS-GO*VDS);
            lastIs = -current;
            break;
    }
    // cout << "n: " << n << " current: " << current << endl << endl;
    return current;
}

double Mosfet::divAtNode(int nin, int dnin) const{
    double VGS = (nodalVoltages[n::G] - nodalVoltages[n::S]) * (NMOS?1:-1);
    double VDS = (nodalVoltages[n::D] - nodalVoltages[n::S]) * (NMOS?1:-1);

    float ID, GM, GO;
    ID = 0;

    if(VGS-VT<0){
        ID = 0;
        GM = 0;
        GO = 0;
    }else if(NMOS?(VGS-VT<VDS):(0<VDS+VGS+VT)){
        ID = K * (VGS-VT)*(VGS-VT) * (hasVA?(1 + VDS/VA):1);
        GM = sqrt(2*K*ID);
        GO = ID/VA;
    }else if(NMOS?(VDS <= VGS-VT):(VDS+VGS+VT<=0)){
        ID = K * (2*(VGS-VT)*VDS-VDS*VDS);
        GM = K*VDS;
        GO = K*((VGS-VT)-VDS);
    }else{
        cerr << "mosfet in a non supported state" << endl;
        exit(1);
    }

    // this is just because I aciddentally set up the switch statement wrong
    // this fixes it, but maybe changing the swtich statement might be more efficient later on
    int n = nin==nodes[n::D]?n::D:(nin==nodes[n::G]?n::G:n::S);
    int dn = dnin==nodes[n::D]?n::D:(dnin==nodes[n::G]?n::G:n::S);

    double conductance;
    switch(n){
        case n::D:
            switch(dn){
                case n::D:
                    conductance = GO;
                    break;
                case n::G:
                    conductance = GM;
                    break;
                case n::S:
                    conductance = -GO-GM;
                    break;
            }
            break;
        case n::G:
            switch(dn){
                case n::D:
                    conductance = 0;
                    break;
                case n::G:
                    conductance = 0;
                    break;
                case n::S:
                    conductance = 0;
                    break;
            }
            break;
        case n::S:
            switch(dn){
                case n::D:
                    conductance = -GO;
                    break;
                case n::G:
                    conductance = -GM;
                    break;
                case n::S:
                    conductance = GO+GM;
                    break;
            }
            break;
    }

    // cout << "n: " << n << " dn: " << dn << " conductance: " << conductance << endl << endl;
    return conductance;
}

string Mosfet::getCurrentHeadingName() const{
    return "id_" + name + ",ig_" + name + ",is_" + name;
}

string Mosfet::getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage, int order) {
    //current through dependent current source + through current source + through resistor
    return to_string(lastId) + "," + to_string(lastIg) + "," + to_string(lastIs);
}