#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <string>
#include <algorithm>

#include "enums.hpp"

using namespace std;

/*
    Acts as interface for circuit components
*/
class Component
{
protected:
    string name;
    string modelName; //name of .model statement
    vector<int> nodes;
    vector<componentType> types; //indicates functionality of subComponents
    vector<float> nodalVoltages;
    int subComponents;
public:
    Component(string _name);
    virtual ~Component() = 0; //destructor of base class should be virtual (purely virtual as component should be abstract)

    virtual float getConductance() const;
    virtual float getVoltage() const;
    virtual float getCurrent() const; //For complex components, this returns the current through the companion model's current source rather than through the whole component
    virtual float getTotalCurrent(float voltage, int order = 1); //For complex components, this return the current through the whole component
    virtual float getGain() const;
    virtual string getVsName() const;
    virtual string getModelName() const;
    
    string getName() const;
    virtual vector<int> getNodes() const = 0;

    // these are used for non linear analysis
    void setNodalVoltages(vector<float> v);
    // get the IV characteristics of a component given two nodes
    virtual float ivAtNode(int n1) const =0;
    // get the derivative of the IV charateristic of a component given two nodes and the node the derivative is based on
    virtual float divAtNode(int n1, int dn) const =0;

    // this should be used to update the value of the voltage and current accross a component after an iteration
    virtual void updateVals(float newVoltage, float newCurrent, int order);
    
    // this should be used to update the time of the time dependant components or update nonlinear components (passing voltage)
    virtual void updateVals(float time_or_voltage);

    // this should be used to set a new timestep for capacitors/inductors
    virtual void setTimeStep(float _timeStep);

    //used to add additional parameters to components after construction
    virtual void addParam(int paramId, float paramValue);

    // helper function to parse string with units into float. Maybe better to move somewhere else
    static float getValue(string val);

    // Transforms nodes from the N123 format into integers
    // The node order is preserved
    vector<int> processNodes(vector<string> rawNodes);
    
    vector<componentType> getTypes() const;
};

#endif
