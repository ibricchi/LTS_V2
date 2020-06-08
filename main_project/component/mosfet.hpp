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

    // used for caluations
    float VGS, VDS, IDEQ, GM, GO;

    //used for current csv output
    float lastId, lastIg, lastIs;
    float lastGd, lastGg, lastGs;
    float lastGo, lastGm;

    enum n{
        D = 0,
        G = 1,
        S = 2
    };

public:
    Mosfet(string _name, vector<string> args, vector<float> extraInfo);
    ~Mosfet() = default;

    void addParam(int paramId, float paramValue) override;
    string getModelName() const override;

    void setNodalVoltages(vector<float> v) override;

    double ivAtNode(int n1) override;
    double divAtNode(int n, int dn) override;

    string getCurrentHeadingName() const override;

    string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif