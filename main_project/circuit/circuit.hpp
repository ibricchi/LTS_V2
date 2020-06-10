#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP

#include <iostream>
#include <vector>
#include <Eigen/Dense>

#include <component/component.hpp>

using namespace std;
using namespace Eigen;

// this struct is used for non-linear analysis
// it makes the consturction of the conductance and source voltages much simpler
struct nodeCompPair{
    int n; // main node being considered
    vector<int> extraNodes; // all other non ground nodes
    Component* comp; // component in question

    double IV() const{ // gets current sources at main node
        return comp->ivAtNode(n);
    }
    double DIV(int dn) const{ // get's conducances with reference to main node and dn
        return comp->divAtNode(n, dn);
    }
};

class Circuit
{
protected:
    string title; // name of circuit

    // these vecotrs improve the efficiency of the analysis by allowing certain functions to only be
    // called on component types that require them
    vector<Component*> components{}; // list of all components in circuit
    vector<Component*> voltageSources{}; // list of all votlage sources or components that will be treated as VS
    vector<Component*> currentSources{}; // list of all current sources or componetns that will be treated as CS
    vector<Component*> conductanceSources{}; // list of all components with conductances
    vector<Component*> vcUpdatables{}; // list of all components that use the updateVoltage methods
    vector<Component*> timeUpdatables{}; // list of all components that need time updates
    vector<Component*> nonVoltageSources{}; // list of all componetns that use IV and DIV
    vector<Component*> nonLinears{}; // list of all non-linear components

    vector<float> timePoints; // time points indicate periods of high change in sourcese, used for dynamic time stepping

    // basic parameters used for all of the simulations
    //all time are is in seconds
    int highestNodeNumber; //more efficient to keep updating when parsing netlist (otherwise have to iterate through all components again)
    double currentTime; // current time in simulation
    double prevTime; // previous simulation time
    float tStart; // starting time, defaults to 0
    float simulationTime; // time when simulation ends (TSTOP)
    float timeStep; // time step between steps, changes dynamically
    float maxTimeStep; // maximum time step to limit dynamic time stepping
    float tStep; // printing incremement for csv output
    bool hasNonLinear; // type of circuit, linear or non-linear
    int inductorNumber; // number of inducntors, used for DC biasing as inductors are replaced with voltage sources, which increase the matrx size
    
    // important matrixes
    MatrixXd A; // conductance matrix
    MatrixXd A_inv; // inverse matrix
    VectorXd b; // source matrix
    VectorXd x; // node voltage, and VS current matrixes
    vector<string> xMeaning; // indicates what the values in x mean (need to know when outputing result)

    double minPNConductance = 1e-12; // minimum conductance at PN juctions

    int maxNewtonRaphsonCount = 500; // set's a maximum itteration cap to newton raphson to detect divergence

    //absolute Newton-Raphson error tolerance
    float abstol = 0.01;

    // used to store all component node pairs to itterate through
    // each of these will be asked for their IV and DIV functions
    vector<nodeCompPair> nodalFunctions{};

public:
    // default constructor for initializing empty object
    Circuit();
    ~Circuit();

    // don't want to have default copy when using pointers
    Circuit(const Circuit&) = delete;
    Circuit& operator=(const Circuit&) = delete;

    // functions used for finding high rates of change in sources for dynamit time stepping
    vector<float>& getTimePoints(); //Returns a vector of PWL timepoints for current/voltage sources
    void setTimePoints();

    // basic stter and getter functions for circuit class
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

    double getMinPNConductance() const;
    void setMinPNConductance(double con);

    int getMaxNewtonRaphsonCount() const;
    void setMaxNewtonRaphsonCount(int count);

    float getAbstol() const;
    void setAbstol(float _abstol);

    // used to controll indcutor number during parsing of input to increase efficiency
    void incrementInductorNumber();
    int getInductorNumber() const;

    // returns references for component type vectors to prevent inefficient copying
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
    // void addComponent(string name, vector<string> args, vector<float> extraInfo);
    // this allows for high levels of polymorphsim in the creation of components
    template <class comp>
    void addComponent(string name, vector<string> args){
        vector<float> extraInfo; // extra info will be passed to constructors and used if necessary
        extraInfo.push_back(getTStep());//extraInfo[0] is timeStep of circuit (currently the printing step tStep is used as the static timestep)
        extraInfo.push_back(getCurrentTime());//extraInfo[1] is current time of circuit
        extraInfo.push_back(voltageSources.size()); //idx of voltageSource inside voltageSources vector (the value will have no meaning if the component is not a voltageSource)
        extraInfo.push_back(getMinPNConductance()); //extraInfo[3] is the minimum allowed conducntacnce at a PN junction
        
        // this part requires the specific constructor on all components specified above
        comp* newComp = new comp(name, args, extraInfo);
        vector<componentType> types = newComp->getTypes();
        for(auto type : types){ // checks the new components type to set up into appropriate vectors
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
            default: // throws error if type is not known
                cerr << "The component " << name << " has no supported componentType" <<endl;
                exit(1);
                break;
            }
        }
        components.push_back(newComp); // aslo adds it to vector of all components
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

    // computes X for non linear circuits
    void computeNLX(float gamma);

    // set X required for dynamit time stepping as it can reset the circuit state to a previous time step
    void setX(VectorXd newX);
    VectorXd getX() const;

    // updates nodal voltages on all components within the circuit
    void updateNodalVoltages();

    //connects a current controlled source with the voltage source that the controlling current flows through
    //the index of the voltage source (in the voltageSources vector) is stored in the nodes vector as node3
    //IMPORTANT: Must call this method before constructing A the first time (application may crash otherwise)
    void setupCurrentControlledSources(Circuit &c);

    // helper function for current controlled sources
    int getVoltageSourceIndexByName(string vsName, vector<Component*>& voltageSources) const;
};

#endif
