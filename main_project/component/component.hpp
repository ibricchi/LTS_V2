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

    // these methods are used for linear analysis construction of A and b matrixes
    virtual float getConductance() const; //Returns conducance through a component
    virtual float getVoltage() const; //Returns voltage across a component
    virtual float getCurrent() const; //For complex components, this returns the current through the companion model's current source rather than through the whole component
    
    // these methods are required for dynamic time stepping, used for finding points were sources change quickly
    virtual vector<float> getTimePoints();
    virtual void setTimePoints();
    
    //get the total current that flows through a component
    //this will be the current that is written to the csv file
    //the current x vector must be passed as an argument (needed to get the current through voltage sources)
    virtual string getTotalCurrentString(const VectorXd &x, int highestNodeNumber, float voltage = 0, int order = 1)  = 0;
    
    virtual float getGain() const; // Returns gain of a component, used for op-amp, and dependnat sources
    virtual string getVsName() const; // Required for current dependant sources
    virtual string getModelName() const;
    
    string getName() const;

    //Get the current heading that will be printed to the CSV file.
    //Should not include the leading and trailing comma.
    //The current heading should be identical to 'i_' + getName() for two terminal devices => Don't need to overload.
    //For >2 terminal devices, the string should include multiple comma seperated values that correspond to the terminals (Used to output currents) => Need to overload.
    virtual string getCurrentHeadingName() const;

    vector<int> getNodes() const; // returns the Nodes vector of a component

    // these are used for non linear analysis
    // updates the nodal voltage vector for a component
    // can be overloaded if necessary, non-linears use this to improve performance
    virtual void setNodalVoltages(vector<float> v);
    // returns the current sources current of the linear model
    virtual double ivAtNode(int n1) =0;
    // returns the conducance between two nodes for a component
    // this includes voltage dependnat current sources, which are effecively conductnaces between to differnt nodes
    virtual double divAtNode(int n, int dn) =0;

    // set minimum PN conductance for non-linear components
    virtual void setMinPNConductance(float con);

    // this should be used to update the value of the voltage and current accross a component after an iteration
    // used for capacitors and inductors
    // more efficient than setNodalVoltages for linear circuits
    // only supports 2 node components
    virtual void updateVals(float newVoltage, float newCurrent, int order);
    
    // this should be used to update the time of the time dependant components or update nonlinear components (passing voltage)
    virtual void updateVals(float time_or_voltage);

    // this should be used to set a new timestep for capacitors/inductors
    // these components require the time step
    virtual void setTimeStep(double _timeStep);

    // used to add additional parameters to components after construction
    // required for model statements
    virtual void addParam(int paramId, float paramValue);

    // helper function to parse string with units into float. Maybe better to move somewhere else
    // static that way it can be used outside of the component class if required
    // for 1k returns 1000 for 1m returns 0.001 etc...
    static float getValue(string val);

    // Transforms nodes from the N123 format into integers
    // The node order is preserved
    vector<int> processNodes(vector<string> rawNodes);
    
    // Returns the types of a components
    // Used to optimize the construction of matrixes, expecially effective in linear circuits
    vector<componentType> getTypes() const;

    //Initialises compCurrent for DC bias capacitors and inductors
    virtual void initCompCurrent(float _voltage_or_current);
    
    // appends a node to the nodes vector
    // used to improve efficiency for dependant sources
    void appendToNodes(int nodeToAppend);

};

#endif
