#ifndef DIODE_HPP
#define DIODE_HPP

#include <vector>
#include <string>

#include "component.hpp"

class Diode: public Component{
private:
    float IS = 1e-14, N=1, VT=0.026;

    void SetupValues(float IS = 1e-14, float N = 1, float VT = 0.025);
public:
    Diode(string _name, vector<string> args, vector<float> extraInfo);
    ~Diode() = default;

    double ivAtNode(int n1) const override;
    double divAtNode(int n1, int dn) const override;

    void addParam(int paramId, float paramValue);
    vector<int> getNodes() const override;
    string getModelName() const override;

    float getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif