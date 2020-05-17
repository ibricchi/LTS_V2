#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <string>

#include "enums.hpp"

using namespace std;

class Component
{
protected:
    string name;
    vector<int> nodes;
    vector<componentType> types; //indicates functionality of subComponents
    int subComponents;
public:
    Component(string _name);
    virtual ~Component() = 0; //destructor of base class should be virtual (purely virtual as component should be abstract)

    virtual float getConductance() const;
    virtual float getVoltage() const;
    virtual float getCurrent() const; //For complex components, this returns the current through the companion model's current source rather than through the whole component
    virtual float getTotalCurrent(float voltage, int order = 1); //For complex components, this return the current through the whole component

    string getName() const;
    virtual vector<int> getNodes() const = 0;

    // these are used for non linear analysis
    // get the IV characteristics of a component at a given ndoe
    virtual vector<pair<int, function<float (float, float)>>> ivAtNode(int node) const =0;
    // get teh derivative of the IV charateristic of a component given a node and the node of which the prime is being taken <-- not sure if that makes sence. Basically the second one is the bottom of the dIV/dV
    virtual vector<pair<int, function<float (float, float)>>> divAtNode(int node, int primeNode) const =0;

    // this should be used to update the value of the voltage and current accross a component after an iteration
    virtual void updateVals(float newVoltage, float newCurrent, int order);
    // this should ebe used to update the time of the time dependant components
    virtual void updateVals(float time);

    float getValue(string val) const;
    vector<componentType> getTypes() const;
};

#endif
