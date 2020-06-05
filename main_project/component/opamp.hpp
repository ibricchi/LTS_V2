#ifndef OPAMP_HPP
#define OPAMP_HPP

#include <vector>
#include <string>

#include "component.hpp"

/*
    Netlist syntax => X<name> <Nin+> <Nin-> <Nout> <modelName>
    The modelName is optional. When no modelName is provided, the Op-Amp is assumed to be ideal.
    An ideal Op-Amp assumes negative feedback exists, Vin+ = Vin- and there is no limit imposed by the supply rails (Can produce infinite voltage).

    Currently, only ideal Op-Amps are supported and the modelName parameter will be ignored!
*/
class OpAmp: public Component{
private:
    string modelName;

    //index of the voltageSource inside the voltageSources vector
    //used for getTotalCurrentString
    int voltageSourcesIdx;
public:
    OpAmp(string _name, vector<string> args, vector<float> extraInfo);
    ~OpAmp() = default;

    double ivAtNode(int n1) override;
    double divAtNode(int n, int dn) override;

    string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif