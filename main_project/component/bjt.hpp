#ifndef BJT_HPP
#define BJT_HPP

#include <vector>
#include <string>

#include "component.hpp"

class BJT: public Component{
private:
    bool NPN = true;
    bool hasVAF = false;
    double BF = 100; // beta forward
    double IRS = 1e-16; // reverse saturation current
    double IFS = 1e-16; // forward saturation current
    double VAF = 100; // forward early voltage
    double BR = 1.0; // beta reverse
    double VT = 0.025; // thermal voltage

    // used for calculations
    double VBE, VBC, VCE, IBF, IBR, IC1, GPF, GPR, GMF, GMR, GO, IC, IB, IE, IBFEQ, IBREQ, ICEQ;

    // don't know if I could replace with enum
    enum n{
        C = 0,
        B = 1,
        E = 2
    };
public:
    BJT(string _name, vector<string> args, vector<float> extraInfo);
    BJT(string _name, float r, int n1, int n2);
    ~BJT() = default;

    double ivAtNode(int n1) override;
    double divAtNode(int n, int dn) override;

    void addParam(int paramId, float paramValue) override;

    void setNodalVoltages(vector<float> v) override;

    string getModelName() const override;

    string getCurrentHeadingName() const override;

    string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif