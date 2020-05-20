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

using namespace std;
using namespace Eigen;

Circuit::Circuit()
{
    currentTime = 0;
    timeStep = 0.1; // in seconds
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

float Circuit::getCurrentTime() const{
    return currentTime;
}
void Circuit::setCurrentTime(float _currentTime){
    currentTime = _currentTime;
}

float Circuit::getSimulationTime() const{
    return simulationTime;
}
void Circuit::setSimulationTime(float _simulationTime){
    simulationTime = _simulationTime;
}

float Circuit::getTimeStep() const{
    return timeStep;
}
void Circuit::setTimeStep(float _timeStep){
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

// setup for non linear
void Circuit::nlSetup(){
    int nvc = nonVoltageSources.size();
    int vsc = voltageSources.size();

    x = VectorXf::Zero(highestNodeNumber + vsc);

    // sets up nodalFunctions vector
    // very similar to the setup of A in linear analysis
    vector<int> nodes;
    vector<int> extraNodes;
    for(Component* comp : nonVoltageSources){
        nodes = comp->getNodes();
        for(int n1 : nodes){
            extraNodes.resize(0);
            if(n1 == 0) continue;
            for(int n2 : nodes){
                if(n1 == n2 || n2 == 0) continue;
                extraNodes.push_back(n2);
            }
            nodalFunctions.push_back(nodeCompPair{n1, extraNodes, comp});
        }            
    }
}

// setupA definition
void Circuit::setupA()
{
    A = MatrixXf::Zero(highestNodeNumber + voltageSources.size(), highestNodeNumber + voltageSources.size());
    vector<int> nodes{};

    //constructing conductance part
    for (const auto &comp : conductanceSources)
    {
        const float conductance = comp->getConductance();
        nodes = comp->getNodes();

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
        const int node1 = nodes.at(0);
        const int node2 = nodes.at(1);

        if (node1 != 0)
        {
            A(node1 - 1, highestNodeNumber + i) = 1;
            A(highestNodeNumber + i, node1 - 1) = 1; //different when dealing with dependent sources
        }

        if (node2 != 0)
        {
            A(node2 - 1, highestNodeNumber + i) = -1;
            A(highestNodeNumber + i, node2 - 1) = -1; //different when dealing with dependent sources
        }
    }
}

void Circuit::nonLinearA(){
    A = MatrixXf::Zero(highestNodeNumber + voltageSources.size(), highestNodeNumber + voltageSources.size());

    // setup currents from non voltage source components
    for(const nodeCompPair ncp : nodalFunctions){
        int n = ncp.n;
        vector<int> extraNodes = ncp.extraNodes;
        
        // nodes are already checked not to be 0 on creation of ncp
        A(n-1, n-1) += ncp.DIV(n);
        for(int en : extraNodes){
            A(n-1, en-1) += ncp.DIV(en);
        }
    }

    // same as linear for VS
    vector<int> nodes;
    for (int i{}; i < voltageSources.size(); i++)
    {
        const auto &vs = voltageSources.at(i);

        nodes = vs->getNodes();
        const int node1 = nodes.at(0);
        const int node2 = nodes.at(1);

        if (node1 != 0)
        {
            A(node1 - 1, highestNodeNumber + i) += 1;
            A(highestNodeNumber + i, node1 - 1) += 1; //different when dealing with dependent sources
        }

        if (node2 != 0)
        {
            A(node2 - 1, highestNodeNumber + i) += -1;
            A(highestNodeNumber + i, node2 - 1) += -1; //different when dealing with dependent sources
        }
    }
}

MatrixXf Circuit::getA() const
{
    return A;
}

void Circuit::computeA_inv(){
    A_inv = A.inverse();
}

MatrixXf Circuit::getA_inv() const{
    return A_inv;
}

// setupB definition
void Circuit::adjustB()
{
    b = VectorXf::Zero(highestNodeNumber + voltageSources.size());

    //adding currents
    for (const auto &cSource : currentSources)
    {
        vector<int> nodes = cSource->getNodes();
        const int node1 = nodes.at(0);
        const int node2 = nodes.at(1);

        // same suggestion as above, would make the whole code base more flexible to new componetns
        if (node1 != 0)
        {
            b(node1 - 1) += cSource->getCurrent();
        }

        if (node2 != 0)
        {
            b(node2 - 1) -= cSource->getCurrent();
        }
    }

    //adding voltages
    for (int i{highestNodeNumber}, j{}; i < highestNodeNumber + voltageSources.size(); i++, j++)
    {
        b(i) = voltageSources.at(j)->getVoltage();
    }
}

void Circuit::nonLinearB(){
    b = VectorXf::Zero(highestNodeNumber + voltageSources.size());

    int n, n1, n2;
    vector<int> nodes, extraNodes;

    // setup currents from non voltage source components
    for(const nodeCompPair ncp : nodalFunctions){
        n = ncp.n;
        extraNodes = ncp.extraNodes;
        b(n-1) += ncp.IV();
    }

    //adding voltages
    for (int i{highestNodeNumber}, j{}; i < highestNodeNumber + voltageSources.size(); i++, j++)
    {
        nodes = voltageSources[j]->getNodes();
        n1 = nodes[0];
        n2 = nodes[1];
        if(n1 != 0) b(n1-1) += x[i];
        if(n2 != 0) b(n2-1) -= x[i];
        // move this part into the IV thing later
        b(i) -= voltageSources.at(j)->getVoltage();
        b(i) += (n1 == 0? 0 : x[n1-1]);
        b(i) -= (n2 == 0? 0 : x[n2-1]);
    }
};
VectorXf Circuit::getB() const
{
    return b;
}

void Circuit::setupXMeaning(){
    xMeaning.clear();

    for(int i{1}; i<=highestNodeNumber; i++){
        xMeaning.push_back("v_" + to_string(i));
    }
    for(const auto &vs : voltageSources){
        xMeaning.push_back("I_V" + vs->getName());
    }
}

vector<string> Circuit::getXMeaning() const{
    return xMeaning;
}

void Circuit::computeX(){
    x = A_inv * b;
}

void Circuit::computeNLX(float gamma){
    x -= gamma * A_inv * b;
}

void Circuit::setX(VectorXf newX){
    x = newX;
}

VectorXf Circuit::getX() const{
    return x;
}

// update nodal voltages
void Circuit::updateNodalVoltages(){
    vector<int> nodes;
    vector<float> newNodalVoltages;
    for(Component* comp : components){
        newNodalVoltages.resize(0);
        nodes = comp->getNodes();
        for(int n : nodes){
            if(n == 0){
                newNodalVoltages.push_back(0);
            }else{
                newNodalVoltages.push_back(x[n-1]);
            }
        }
        comp->setNodalVoltages(newNodalVoltages);
    }
};