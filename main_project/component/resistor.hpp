#ifndef RESISTOR_HPP
#define RESISTOR_HPP

#include <vector>
#include <string>

#include "component.hpp"

class Resistor: public Component{
private:
    double conductance;
public:
    Resistor(string _name, vector<string> args, vector<double> extraInfo);
    Resistor(string _name, double r, int n1, int n2);
    ~Resistor() = default;

    double getConductance() const override;
    vector<int> getNodes() const override;
};

#endif