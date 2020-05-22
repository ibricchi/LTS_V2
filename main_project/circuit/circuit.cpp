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

void Circuit::incrementInductorNumber(){
    inductorNumber++;
}

vector<Component*>& Circuit::getComponentsRef(){
    return components;
}

Component* Circuit::getLastComponent(){
    return components[components.size()-1];
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
void Circuit::setupA(bool dc)
{
    int extraZeroVSNumber = dc ? inductorNumber : 0;
    A = MatrixXf::Zero(highestNodeNumber + voltageSources.size() + extraZeroVSNumber, highestNodeNumber + voltageSources.size() + extraZeroVSNumber);
    vector<int> nodes{};

    //constructing conductance part
    for (const auto &comp : conductanceSources)
    {
        //Replace capacitors/inductors with open circuit for DC operating point analysis
        if(dc && (typeid(*comp) == typeid(Capacitor) || typeid(*comp) == typeid(Inductor))){
            //Capacitor is open circuit at DC => don't contribute conductance
            //Inductor is short circuit at DC => equivalent to zero voltage source (added after all normal voltage sources)
            continue; 
        }

        float conductance = comp->getConductance();
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
            float gain = vs->getGain();
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
            float gain = vs->getGain();
            int controllingVsIndex = getVoltageSourceIndexByName(vs->getVsName(), voltageSources);

            A(highestNodeNumber + i, highestNodeNumber + controllingVsIndex) -= gain;
        }
    }

    //Important that the following loop comes after all normal voltage sources have already been added!
    if(dc){
        int i = voltageSources.size(); //Add zero voltage sources after normal voltage sources
        for(const auto& comp : vcUpdatables){
            if(typeid(*comp) == typeid(Inductor)){
                //add zero voltage source to represent short circuit
                nodes = comp->getNodes();
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
            }
            i++;
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
            
            float gain = cs->getGain();
            
            if(node1 != 0 && nodeC1 != 0){
                A(node1 - 1, nodeC1 - 1) += gain;
            }
            if(node1 != 0 && nodeC2 != 0){
                A(node1 - 1, nodeC2 - 1) -= gain;
            }
            if(node2 != 0 && nodeC1 != 0){
                A(node2 - 1, nodeC1 - 1) -= gain;
            }
            if(node2 != 0 && nodeC2 != 0){
                A(node2 - 1, nodeC2 - 1) += gain;
            }
        }else if(typeid(*cs) == typeid(CurrentControlledCurrentSource)){
            nodes = cs->getNodes();
            int node1 = nodes.at(0);
            int node2 = nodes.at(1);

            float gain = cs->getGain();
            int controllingVsIndex = getVoltageSourceIndexByName(cs->getVsName(), voltageSources);

            if(node1 != 0){
                A(node1 - 1, highestNodeNumber + controllingVsIndex) += gain;
            }
            if(node2 != 0){
                A(node2 - 1, highestNodeNumber + controllingVsIndex) -= gain;
            }
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

MatrixXf Circuit::getA_inv() const{
    return A_inv;
}

// setupB definition
void Circuit::adjustB(bool dc)
{
    int extraZeroVSNumber = dc ? inductorNumber : 0;
    b = VectorXf::Zero(highestNodeNumber + voltageSources.size() + extraZeroVSNumber);

    //adding currents
    for (const auto &cs : currentSources)
    {
        if(dc && (typeid(*cs) == typeid(Capacitor) || typeid(*cs) == typeid(Inductor))){
            continue; // Open/short circuit at DC => Don't contribute currents
        }

        //dependent current sources don't contribute to the b vector
        if(typeid(*cs) == typeid(CurrentControlledCurrentSource) || typeid(*cs) == typeid(VoltageControlledCurrentSource)){ 
            continue;
        }

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
}