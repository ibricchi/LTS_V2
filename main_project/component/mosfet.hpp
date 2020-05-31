#ifndef Mosfet_HPP
#define Mosfet_HPP

#include <vector>
#include <string>

#include "component.hpp"

class Mosfet: public Component{
private:
    float K = 2e-5;
    float VT = 0;
    float VA = 1; // really should be infinite but won't be used unless hasVA is true
    bool hasVA = false;
    bool NMOS = true;

    enum n{
        D = 0,
        G = 1,
        S = 2
    };

    void SetupValues();
public:
    Mosfet(string _name, vector<string> args, vector<float> extraInfo);
    ~Mosfet() = default;

    void addParam(int paramId, float paramValue) override;
    string getModelName() const override;

    double ivAtNode(int n1) const override;
    double divAtNode(int n1, int dn) const override;

    string getCurrentHeadingName() const override;

    string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif