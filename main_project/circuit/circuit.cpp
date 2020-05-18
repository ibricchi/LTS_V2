#include <iostream>
#include <Eigen/Dense>
#include <vector>

#include "circuit.hpp"

#include <component/enums.hpp>
#include <component/component.hpp>
#include <component/voltageSource.hpp>
#include <component/currentSource.hpp>
#include <component/resistor.hpp>
#include <component/capacitor.hpp>
#include <component/inductor.hpp>
#include <component/voltageControlledVoltageSource.hpp>
#include <component/currentControlledVoltageSource.hpp>
#include <component/voltageControlledCurrentSource.hpp>
#include <component/currentControlledCurrentSource.hpp>

using namespace std;
using namespace Eigen;

Circuit::Circuit()
{
    currentTime = 0;
    timeStep = 0.001; // in seconds
    highestNodeNumber = 0;
    hasNonLinear = false;
}

//dealocate raw pointers
Circuit::~Circuit()
{
    for(auto comp : components){
        delete comp;
    }
}

string Circuit::getTitle() const{
    return title;
}
void Circuit::setTitle(string _title){
    title = _title;
}

int Circuit::getHighestNodeNumber() const{
    return highestNodeNumber;
}
void Circuit::setHighestNodeNumber(int _highestNodeNumber){
    highestNodeNumber = _highestNodeNumber;
}

double Circuit::getCurrentTime() const{
    return currentTime;
}
void Circuit::setCurrentTime(double _currentTime){
    currentTime = _currentTime;
}

double Circuit::getSimulationTime() const{
    return simulationTime;
}
void Circuit::setSimulationTime(double _simulationTime){
    simulationTime = _simulationTime;
}

double Circuit::getTimeStep() const{
    return timeStep;
}
void Circuit::setTimeStep(double _timeStep){
    timeStep = _timeStep;
}

bool Circuit::hasNonLinearComponents() const{
    return hasNonLinear;
}
void Circuit::setHasNonLinearComponents(bool _hasNonLinearComponents){
    hasNonLinear = _hasNonLinearComponents;
}

vector<Component*>& Circuit::getVoltageSourcesRef(){
    return voltageSources;
}
vector<Component*>& Circuit::getCurrentSourcesRef(){
    return currentSources;
}
vector<Component*>& Circuit::getConductanceSourcesRef(){
    return conductanceSources;
}
vector<Component*>& Circuit::getVCUpdatablesRef(){
    return vcUpdatables;
}
vector<Component*>& Circuit::getTimeUpdatablesRef(){
    return timeUpdatables;
}
vector<Component*>& Circuit::getNonLinearsRef(){
    return nonLinears;
}

// setupA definition
void Circuit::setupA()
{
    A = MatrixXd::Zero(highestNodeNumber + voltageSources.size(), highestNodeNumber + voltageSources.size());
    vector<int> nodes{};

    //constructing conductance part
    for (const auto &comp : conductanceSources)
    {
        const double conductance = comp->getConductance();
        nodes = comp->getNodes();

        // I had to change the algorithm to allow for node1 to be 0
        // Doing it this way also allows for multiple nodes per component
        // This code would get much cleaner if we have a node input to the getConductnace method to return a positive or negative depending on the node order
        // But that's just a thought for the future

        // Isn't loop less efficient than using if statements? (Main objectives of LTS_V2 are accuracy and efficiency)
        // => Second loop will face multiple scenarios when we just continue. This is wasted time. Old method would just use two if statements with no going in circles (loop)
        // Not really significant for small circuits that don't consider nonlinear components
        // but could become significant for very large circuits and if we have to recompute A (kinda depends on the method we choose) for nonlinear components
        // For flexibility: I still cannot think of an example where there is a 3+ terminal device that would work with this exact algorithm (if there is, this would make the code even less efficient)
        for (int i = 0; i < nodes.size(); i++)
        {
            if (nodes[i] == 0)
                continue;
            A(nodes[i] - 1, nodes[i] - 1) += conductance;
            for (int j = 0; j < nodes.size(); j++)
            {
                if (i == j || nodes[j] == 0)
                    continue;
                A(nodes[i] - 1, nodes[j] - 1) -= conductance;
            }
        }
    }

    //voltage part
    for (int i{}; i < voltageSources.size(); i++)
    {
        const auto &vs = voltageSources.at(i);

        nodes = vs->getNodes();
        int node1 = nodes.at(0);
        int node2 = nodes.at(1);

        if (node1 != 0)
        {
            A(node1 - 1, highestNodeNumber + i) = 1;
            A(highestNodeNumber + i, node1 - 1) = 1;
        }

        if (node2 != 0)
        {
            A(node2 - 1, highestNodeNumber + i) = -1;
            A(highestNodeNumber + i, node2 - 1) = -1;
        }
        
        // need to add additional values when controlled sources
        if(typeid(*vs) == typeid(VoltageControlledVoltageSource)){
            double gain = vs->getGain();
            int nodeC1 = nodes.at(2);
            int nodeC2 = nodes.at(3);

            if (nodeC1 != 0)
            {
                A(highestNodeNumber + i, nodeC1 - 1) -= gain;
            }

            if (nodeC2 != 0)
            {
                A(highestNodeNumber + i, nodeC2 - 1) += gain;
            }
        }else if(typeid(*vs) == typeid(CurrentControlledVoltageSource)){
            double gain = vs->getGain();
            int controllingVsIndex = getVoltageSourceIndexByName(vs->getVsName(), voltageSources);

            A(highestNodeNumber + i, highestNodeNumber + controllingVsIndex) -= gain;
        }
    }

    //dependent current sources
    for(const auto& cs : currentSources){
        if(typeid(*cs) == typeid(VoltageControlledCurrentSource)){
            nodes = cs->getNodes();
            int node1 = nodes.at(0);
            int node2 = nodes.at(1);
            int nodeC1 = nodes.at(2);
            int nodeC2 = nodes.at(3);
            
            double gain = cs->getGain();
            
            A(node1 - 1, nodeC1 - 1) += gain;
            A(node1 - 1, nodeC2 - 1) -= gain;
            A(node2 - 1, nodeC1 - 1) -= gain;
            A(node2 - 1, nodeC2 - 1) += gain;
        }else if(typeid(*cs) == typeid(CurrentControlledCurrentSource)){
            nodes = cs->getNodes();
            int node1 = nodes.at(0);
            int node2 = nodes.at(1);

            double gain = cs->getGain();
            int controllingVsIndex = getVoltageSourceIndexByName(cs->getVsName(), voltageSources);

            A(node1 - 1, highestNodeNumber + controllingVsIndex) += gain;
            A(node2 - 1, highestNodeNumber + controllingVsIndex) -= gain;
        }
    }
}

MatrixXd Circuit::getA() const
{
    return A;
}

int Circuit::getVoltageSourceIndexByName(string vsName, vector<Component*>& voltageSources) const{
    for(int i{}; i<voltageSources.size(); i++){
        if(voltageSources.at(i)->getName() == vsName){
            return i;
        }
    }
}

void Circuit::computeA_inv(){
    A_inv = A.inverse();
}

MatrixXd Circuit::getA_inv() const{
    return A_inv;
}

// setupB definition
void Circuit::adjustB()
{
    b = VectorXd::Zero(highestNodeNumber + voltageSources.size());

    //adding currents
    for (const auto &cs : currentSources)
    {
        if(typeid(*cs) == typeid(CurrentSource)){
            vector<int> nodes = cs->getNodes();
            const int node1 = nodes.at(0);
            const int node2 = nodes.at(1);

            // same suggestion as above, would make the whole code base more flexible to new componetns
            if (node1 != 0)
            {
                b(node1 - 1) += cs->getCurrent();
            }

            if (node2 != 0)
            {
                b(node2 - 1) -= cs->getCurrent();
            }
        }
    }

    //adding voltages
    for (int i{highestNodeNumber}, j{}; i < highestNodeNumber + voltageSources.size(); i++, j++)
    {
        auto vs = voltageSources.at(j);

        if(typeid(*vs) == typeid(VoltageControlledVoltageSource) || typeid(*vs) == typeid(CurrentControlledVoltageSource)){
            continue;
        }else{ // normal/independent voltage sources
            b(i) = vs->getVoltage();
        }
    }
}

VectorXd Circuit::getB() const
{
    return b;
}

void Circuit::setupXMeaning(){
    xMeaning.clear();

    for(int i{1}; i<=highestNodeNumber; i++){
        xMeaning.push_back("v_" + to_string(i));
    }

    for(const auto &vs : voltageSources){
        if(typeid(*vs) == typeid(VoltageControlledVoltageSource)){
            xMeaning.push_back("I_E" + vs->getName());
        }else{ // normal/independent voltage sources
            xMeaning.push_back("I_V" + vs->getName());
        }
    }
}

vector<string> Circuit::getXMeaning() const{
    return xMeaning;
}

void Circuit::computeX(){
    x = A_inv * b;
}

VectorXd Circuit::getX() const{
    return x;
}
