#ifndef BJT_HPP
#define BJT_HPP

#include <vector>
#include <string>

#include "component.hpp"

class BJT: public Component{
private:
    bool NPN = true; // controls wheter NPN or PNP mosfet is simulated
    bool hasVAF = false; // controls early voltage simulation is used or not
    double BF = 100; // beta forward
    double IRS = 1e-16; // reverse saturation current
    double IFS = 1e-16; // forward saturation current
    double VAF = 100; // forward early voltage (not used unless a value is actully given, 100 default doesn't matter)
    double BR = 1.0; // beta reverse
    double VT = 0.025; // thermal voltage

    // used for calculations of curents and conducances
    double VBE, VBC, VCE, IBF, IBR, IC1, GPF, GPR, GMF, GMR, GO, IC, IB, IE, IBFEQ, IBREQ, ICEQ;

    double minPNConductance; // minimum allowed conductance GMIN

    // used to easily work with nodes
    enum n{
        C = 0, // collector
        B = 1, // base
        E = 2 // emitter
    };
public:
    // constrcutor with arguments and extra info
    BJT(string _name, vector<string> args, vector<float> extraInfo);
    ~BJT() = default;

    // returns current sources effect on node
    double ivAtNode(int n1) override;
    // returns conducance between two nodes
    // voltage dependant current sources included, as they are a condcutance between two different nodes
    double divAtNode(int n, int dn) override;

    // enables model command to be used on this component
    void addParam(int paramId, float paramValue) override;
    // allows options parameter to alter the GMIN for the component
    void setMinPNConductance(float con) override;

    // overloaded to improve effiiency at each update of voltages
    // curerent sources and condcuances are reqcaluclated once and avaialble for IV and DIV functions
    // prevents the need to re-calculate the values at each call
    void setNodalVoltages(vector<float> v) override;
    
    // supports component models
    string getModelName() const override;

    // overloads current outpt functions as 3 nodes are present and default function only supports 2
    string getCurrentHeadingName() const override;
    string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif