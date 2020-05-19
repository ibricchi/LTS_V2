#ifndef BJT_HPP
#define BJT_HPP

#include <vector>
#include <string>

#include "component.hpp"

class BJT: public Component{
private:
    bool hasVAF;
    float BF, AF, IS, VAF;
    float VT = 0.025;

    // don't know if I could replace with enum
    enum n{
        C = 0,
        B = 1,
        E = 2
    };

    void SetupValues(float BF = 100, float IS = 1e-16, bool hasVAF = false, float VAF = 0);
public:
    BJT(string _name, vector<string> args, vector<float> extraInfo);
    BJT(string _name, float r, int n1, int n2);
    ~BJT() = default;

    float ivAtNode(int n1) const override;
    float divAtNode(int n1, int dn) const override;

    vector<int> getNodes() const override;
};

#endif