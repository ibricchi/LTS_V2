#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <Eigen/Dense>

#include <cmath> //temporarily for returning nanf("") => remove later on

#include "enums.hpp"

class Circuit; //forward declaration avoids problems with circular references
using namespace std;
using namespace Eigen;

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
    
    //get the total current that flows through a component
    //this will be the current that is written to the csv file
    //the current x vector must be passed as an argument (needed to get the current through voltage sources)
    virtual float getTotalCurrent(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  = 0;
    
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
    virtual void setTimeStep(double _timeStep);

    //used to add additional parameters to components after construction
    virtual void addParam(int paramId, float paramValue);

    // helper function to parse string with units into float. Maybe better to move somewhere else
    static float getValue(string val);

    // Transforms nodes from the N123 format into integers
    // The node order is preserved
    vector<int> processNodes(vector<string> rawNodes);
    
    vector<componentType> getTypes() const;

//Initialises compCurrent for DC bias capacitors and inductors
    virtual void initCompCurrent(float _voltage_or_current);
    //appends a node to the nodes vector
    void appendToNodes(int nodeToAppend);
};

#endif
