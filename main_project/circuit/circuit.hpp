#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP

#include <iostream>
#include <vector>
#include <Eigen/Dense>

#include <component/component.hpp>

using namespace std;
using namespace Eigen;

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
    vector<Component*> nonLinears{};
    int highestNodeNumber; //more efficient to keep updating when parsing netlist (otherwise have to iterate through all components again)
    //all time is in seconds
    double currentTime;
    double simulationTime; //time when simulation ends
    double timeStep;
    bool hasNonLinear;
    MatrixXd A;
    MatrixXd A_inv;
    VectorXd b;
    VectorXd x;
    vector<string> xMeaning; // indicates what the values in x mean (need to know when outputing result)
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

    double getSimulationTime() const;
    void setSimulationTime(double _simulationTime);

    double getTimeStep() const;
    void setTimeStep(double _timeStep);

    bool hasNonLinearComponents() const;
    void setHasNonLinearComponents(bool _hasNonLinearComponents);

    // returns references to prevent inefficient copying
    vector<Component*>& getVoltageSourcesRef();
    vector<Component*>& getCurrentSourcesRef();
    vector<Component*>& getConductanceSourcesRef();
    vector<Component*>& getVCUpdatablesRef();
    vector<Component*>& getTimeUpdatablesRef();
    vector<Component*>& getNonLinearsRef();

    // operator overload to add ability to read from iostream to set up circuit
    void operator<<(istream& input);

    // template function to add component, the class must have a constructor with the intputs as in the function bellow
    // template <class comp>
    // void addComponent(string name, vector<string> args);
    template <class comp>
    void addComponent(string name, vector<string> args){
        vector<double> extraInfo; // extra info will be passed to constructors and used if necessary
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
            case componentType::nonLinear:
                nonLinears.push_back(newComp);
                hasNonLinear = true;
                break;
            default:
                cerr << "Unknown componentType" <<endl;
                exit(1); //change to error
                break;
            }
        }
        components.push_back(newComp);
    }

    // operation to create A
    void setupA();
    MatrixXd getA() const;

    // helper function for current controlled sources
    int getVoltageSourceIndexByName(string vsName, vector<Component*>& voltageSources) const;

    // compute inverse of A
    void computeA_inv();
    MatrixXd getA_inv() const;

    // operation to adjust B
    void adjustB();
    VectorXd getB() const;

    // operation to assign meaning to the result vector x
    void setupXMeaning();
    vector<string> getXMeaning() const;

    // A_inv must exist for this to work
    void computeX();
    VectorXd getX() const;
};

#endif