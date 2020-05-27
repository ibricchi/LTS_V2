#ifndef DIODE_HPP
#define DIODE_HPP

#include <vector>
#include <string>

#include "component.hpp"

class Diode: public Component{
private:
    float IS, N, VT;

    void SetupValues(float IS = 1e-14, float N = 1, float VT = 0.025);
public:
    Diode(string _name, vector<string> args, vector<float> extraInfo);
    Diode(string _name, float r, int n1, int n2);
    ~Diode() = default;

    float ivAtNode(int n1) const override;
    float divAtNode(int n1, int dn) const override;

    vector<int> getNodes() const override;

    float getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  override;
};

#endif