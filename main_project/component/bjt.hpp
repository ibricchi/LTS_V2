#ifndef BJT_HPP
#define BJT_HPP

#include <vector>
#include <string>

#include "component.hpp"

class BJT: public Component{
private:
    bool hasVAF;
    float BF, AF, IRS, IFS, VAF;
    float BR = 1.0;
    float VT = 0.025;

    // don't know if I could replace with enum
    enum n{
        C = 0,
        B = 1,
        E = 2
    };

    void SetupValues(float BF = 100, float IFS = 1e-16, bool hasVAF = false, float VAF = 100);
public:
    BJT(string _name, vector<string> args, vector<float> extraInfo);
    BJT(string _name, float r, int n1, int n2);
    ~BJT() = default;

    double ivAtNode(int n1) const override;
    double divAtNode(int n1, int dn) const override;

    float getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif