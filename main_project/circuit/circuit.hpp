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

    double IV() const{
        return comp->ivAtNode(n);
    }
    double DIV(int dn) const{
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
    double currentTime;
    double prevTime;
    float tStart;
    float simulationTime; //time when simulation ends (TSTOP)
    float timeStep;
    float maxTimeStep;
    float tStep; //printing incremement for csv output
    bool hasNonLinear;
    int inductorNumber;
    MatrixXd A;
    MatrixXd A_inv;
    VectorXd b;
    VectorXd x;
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

    double getCurrentTime() const;
    void setCurrentTime(double _currentTime);

    double getPrevTime() const;
    void setPrevTime(double _prevTime);

    float getTStart() const;
    void setTStart(float _tStart);

    float getMaxTimeStep() const;
    void setMaxTimeStep(float _maxTimeStep);

    float getTStep() const;
    float setTStep(float _tStep);

    float getSimulationTime() const;
    void setSimulationTime(float _simulationTime);

    float getTimeStep() const;
    void setTimeStep(float _timeStep);

    bool hasNonLinearComponents() const;
    void setHasNonLinearComponents(bool _hasNonLinearComponents);


    void incrementInductorNumber();

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
        extraInfo.push_back(getTStep());//extraInfo[0] is timeStep of circuit (currently the printing step tStep is used as the static timestep)
        extraInfo.push_back(getCurrentTime());//extraInfo[1] is current time of circuit
        extraInfo.push_back(voltageSources.size()); //idx of voltageSource inside voltageSources vector (the value will have no meaning if the component is not a voltageSource)
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
                nonLinears.push_back(newComp);
                hasNonLinear = true;
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
    void nlSetup(bool isDc = false);

    // operation to create A
    void setupA(bool isDc = false);

    // non linear A
    void nonLinearA(bool isDc = false);

    MatrixXd getA() const;

    // compute inverse of A
    void computeA_inv();
    MatrixXd getA_inv() const;

    // operation to adjust B
    void adjustB(bool isDc = false);

    // non linear b
    //alpha is the source value multiplier. It is 1 for normal operation and 0<=x<=1 for source stepping
    void nonLinearB(bool isDc = false, float alpha = 1);

    VectorXd getB() const;

    // operation to assign meaning to the result vector x
    void setupXMeaning();
    vector<string> getXMeaning() const;

    // A_inv must exist for this to work
    void computeX();

    void computeNLX(float gamma);

    void setX(VectorXd newX);
    
    VectorXd getX() const;

    // update nodal voltages
    void updateNodalVoltages();

    //connects a current controlled source with the voltage source that the controlling current flows through
    //the index of the voltage source (in the voltageSources vector) is stored in the nodes vector as node3
    //IMPORTANT: Must call this method before constructing A the first time (application may crash otherwise)
    void setupCurrentControlledSources(Circuit &c);

    // helper function for current controlled sources
    int getVoltageSourceIndexByName(string vsName, vector<Component*>& voltageSources) const;

    int getInductorNumber() const;
};

#endif
