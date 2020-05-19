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


    // this should be used to update the value of the voltage and current accross a component after an iteration
    virtual void updateVals(float newVoltage, float newCurrent, int order);
    // this should ebe used to update the time of the time dependant components
    virtual void updateVals(float time);

    //used to add additional parameters to components after construction
    virtual void addParam(int paramId, float paramValue);

    // helper function to parse string with units into float. Maybe better to move somewhere else
    static float getValue(string val);
    vector<componentType> getTypes() const;
};

#endif
