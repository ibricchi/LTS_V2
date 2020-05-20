#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP

#include <iostream>
#include <vector>
#include <Eigen/Dense>

#include <component/component.hpp>

using namespace std;
using namespace Eigen;

struct nodeCompPair{
    int n;
    vector<int> extraNodes;
    Component* comp;

    float IV() const{
        return comp->ivAtNode(n);
    }
    float DIV(float dn) const{
        return comp->divAtNode(n, dn);
    }
};

class Circuit
{
protected:
    string title;
    vector<Component*> components{}; // not sure if we necesarily need this one
    vector<Component*> voltageSources{};
    vector<Component*> currentSources{};
    vector<Component*> conductanceSources{};
    vector<Component*> vcUpdatables{};
    vector<Component*> timeUpdatables{};
    vector<Component*> nonVoltageSources{};
    vector<Component*> nonLinears{};

    int highestNodeNumber; //more efficient to keep updating when parsing netlist (otherwise have to iterate through all components again)
    //all time is in seconds
    float currentTime;
    float simulationTime; //time when simulation ends
    float timeStep;
    bool hasNonLinear;
    MatrixXf A;
    MatrixXf A_inv;
    VectorXf b;
    VectorXf x;
    vector<string> xMeaning; // indicates what the values in x mean (need to know when outputing result)

    // non-linear analysis vectors;
    vector<nodeCompPair> nodalFunctions{};
public:
    // default constructor for initializing empty object
    Circuit();
    ~Circuit();

    //don't want to have default copy when using pointers (not even allowed to copy unique ptr)
    Circuit(const Circuit&) = delete;
    Circuit& operator=(const Circuit&) = delete;

    string getTitle() const;
    void setTitle(string _title);

    int getHighestNodeNumber() const;
    void setHighestNodeNumber(int _highestNodeNumber);

    float getCurrentTime() const;
    void setCurrentTime(float _currentTime);

    float getSimulationTime() const;
    void setSimulationTime(float _simulationTime);

    float getTimeStep() const;
    void setTimeStep(float _timeStep);

    bool hasNonLinearComponents() const;
    void setHasNonLinearComponents(bool _hasNonLinearComponents);

    // returns references to prevent inefficient copying
    vector<Component*>& getComponentsRef();
    Component* getLastComponent();
    vector<Component*>& getVoltageSourcesRef();
    vector<Component*>& getCurrentSourcesRef();
    vector<Component*>& getConductanceSourcesRef();
    vector<Component*>& getVCUpdatablesRef();
    vector<Component*>& getTimeUpdatablesRef();
    vector<Component*>& getNonLinearsRef();

    // template function to add component, the class must have a constructor with the intputs as in the function bellow
    // template <class comp>
    // void addComponent(string name, vector<string> args);
    template <class comp>
    void addComponent(string name, vector<string> args){
        vector<float> extraInfo; // extra info will be passed to constructors and used if necessary
        // we can change it to a vector of strings if we need non float data later on
        extraInfo.push_back(getTimeStep());//extraInfo[0] is timeStep of circuit
        extraInfo.push_back(getCurrentTime());//extraInfo[1] is current time of circuit
        comp* newComp = new comp(name, args, extraInfo);
        vector<componentType> types = newComp->getTypes();
        for(auto type : types){
            switch (type)
            {
            case componentType::conductanceSource:
                conductanceSources.push_back(newComp);
                break;
            case componentType::voltageSource:
                voltageSources.push_back(newComp);
                break;
            case componentType::currentSource:
                currentSources.push_back(newComp);
                break;
            case componentType::vcUpdatable:
                vcUpdatables.push_back(newComp);
                break;
            case componentType::timeUpdatable:
                timeUpdatables.push_back(newComp);
                break;
            case componentType::nonVoltageSource:
                nonVoltageSources.push_back(newComp);
                break;
            case componentType::nonLinear:
                hasNonLinear = true;
                nonLinears.push_back(newComp);
                break;
            default:
                cerr << "The component " << name << " has no supported componentType" <<endl;
                exit(1);
                break;
            }
        }
        components.push_back(newComp);
    }

    // non linear setup
    void nlSetup();

    // operation to create A
    void setupA();
    // non linear A
    void nonLinearA();
    MatrixXf getA() const;

    // compute inverse of A
    void computeA_inv();
    MatrixXf getA_inv() const;

    // operation to adjust B
    void adjustB();
    // non linear b
    void nonLinearB();
    VectorXf getB() const;

    // operation to assign meaning to the result vector x
    void setupXMeaning();
    vector<string> getXMeaning() const;

    // A_inv must exist for this to work
    void computeX();
    void computeNLX(float gamma);
    void setX(VectorXf newX);
    VectorXf getX() const;

    // update nodal voltages
    void updateNodalVoltages();
};

#endif