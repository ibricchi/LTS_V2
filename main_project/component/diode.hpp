#ifndef DIODE_HPP
#define DIODE_HPP

#include <vector>
#include <string>

#include "component.hpp"

class Diode: public Component{
private:
    float IS = 1e-14, N=1, VT=0.026;

    //Used to prevent recomputing exponentials for getTotalCurrentString
    double lastIeq, lastConductance;

    //Used for calculations
    double v, current, conductance;

    double minPNConductance; // minimum allowed conductance

    void SetupValues(float IS = 1e-14, float N = 1, float VT = 0.025);
public:
    Diode(string _name, vector<string> args, vector<float> extraInfo);
    ~Diode() = default;

    double ivAtNode(int n1) override;
    double divAtNode(int n, int dn) override;

    void setNodalVoltages(vector<float> nv) override;
    void setMinPNConductance(float con) override;

    void addParam(int paramId, float paramValue);
    string getModelName() const override;

    string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif