#ifndef Mosfet_HPP
#define Mosfet_HPP

#include <vector>
#include <string>

#include "component.hpp"

class Mosfet: public Component{
private:
    bool hasVAF;
    float BF, AF, IRS, IFS, VAF;
    float BR = 1.0;
    float VT = 0.025;

    // don't know if I could replace with enum
    enum n{
        D = 0,
        G = 1,
        S = 2
    };

    void SetupValues();
public:
    Mosfet(string _name, vector<string> args, vector<float> extraInfo);
    Mosfet(string _name, float r, int n1, int n2);
    ~Mosfet() = default;

    float ivAtNode(int n1) const override;
    float divAtNode(int n1, int dn) const override;

    vector<int> getNodes() const override;
};

#endif