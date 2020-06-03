#ifndef BJT_HPP
#define BJT_HPP

#include <vector>
#include <string>

#include "component.hpp"

class BJT: public Component{
private:
    bool hasVAF;
    double BF, AF, IRS, IFS, VAF;
    double BR = 1.0;
    double VT = 0.025;

    //used for current csv output
    double lastIc, lastIb, lastIe;

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

    double ivAtNode(int n1) override;
    double divAtNode(int n, int dn) override;

    string getModelName() const override;

    string getCurrentHeadingName() const override;

    string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif