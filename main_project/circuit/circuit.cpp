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
#include <component/opamp.hpp>

using namespace std;
using namespace Eigen;

Circuit::Circuit()
{
    currentTime = 0;
    timeStep = 0.001; // in seconds
    highestNodeNumber = 0;
    inductorNumber = 0;
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

float Circuit::getTStart() const{
    return tStart;
}
void Circuit::setTStart(float _tStart){
    tStart = _tStart;
}

float Circuit::getMaxTimeStep() const{
    return maxTimeStep;
}
void Circuit::setMaxTimeStep(float _maxTimeStep){
    maxTimeStep = _maxTimeStep;
}

float Circuit::getTStep() const{
    return tStep;
}
float Circuit::setTStep(float _tStep){
    tStep = _tStep;
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

int Circuit::getInductorNumber()const{
return inductorNumber;
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
void Circuit::nlSetup(bool isDc){
    int nvc = nonVoltageSources.size();
    int vsc = voltageSources.size();
    int extraZeroVSNumber = isDc ? inductorNumber : 0;

    x = VectorXd::Zero(highestNodeNumber + vsc + extraZeroVSNumber);

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
void Circuit::setupA(bool isDc)
{
    //cerr <<"Inductor number: "<< inductorNumber << endl;
    int extraZeroVSNumber = isDc ? inductorNumber : 0;
    A = MatrixXd::Zero(highestNodeNumber + voltageSources.size() + extraZeroVSNumber, highestNodeNumber + voltageSources.size() + extraZeroVSNumber);
    vector<int> nodes{};

    //constructing conductance part
    for (const auto &comp : conductanceSources)
    {
        //Replace capacitors/inductors with open circuit for DC operating point analysis
        if(isDc && (typeid(*comp) == typeid(Capacitor) || typeid(*comp) == typeid(Inductor))){
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

        //slightly different for ideal opamp
        if(typeid(*vs) == typeid(OpAmp)){
            nodes = vs->getNodes();
            int node1 = nodes.at(0); //Nin+
            int node2 = nodes.at(1); //Nin-
            int node3 = nodes.at(2); //Nout

            if (node1 != 0)
            {
                A(highestNodeNumber + i, node1 - 1) = 1;
            }

            if (node2 != 0)
            {
                A(highestNodeNumber + i, node2 - 1) = -1;
            }

            if (node3 != 0)
            {
                A(node3 - 1, highestNodeNumber + i) = 1;
            }

            continue; //Rest doesn't apply to ideal opamps
        }

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
            int controllingVsIndex = nodes.at(2); //idx of controlling source added here during setup

            A(highestNodeNumber + i, highestNodeNumber + controllingVsIndex) -= gain;
        }
    }

    //Important that the following loop comes after all normal voltage sources have already been added!
    if(isDc){
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
                i++;
	        }
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
            int controllingVsIndex = nodes.at(2); //idx of controlling source added here during setup

            if(node1 != 0){
                A(node1 - 1, highestNodeNumber + controllingVsIndex) += gain;
            }
            if(node2 != 0){
                A(node2 - 1, highestNodeNumber + controllingVsIndex) -= gain;
            }
        }
    }
}

void Circuit::nonLinearA(bool isDc){
    int extraZeroVSNumber = isDc ? inductorNumber : 0;

    A = MatrixXd::Zero(highestNodeNumber + voltageSources.size() + extraZeroVSNumber, highestNodeNumber + voltageSources.size() + extraZeroVSNumber);

    // setup currents from non voltage source components
    int n{};
    vector<int> extraNodes{};
    double conductance{};
    for(const nodeCompPair ncp : nodalFunctions){
        //Replace capacitors/inductors with open circuit for DC operating point analysis
        if(isDc && (typeid(*ncp.comp) == typeid(Capacitor) || typeid(*ncp.comp) == typeid(Inductor))){
            //Capacitor is open circuit at DC => don't contribute conductance
            //Inductor is short circuit at DC => equivalent to zero voltage source (added after all normal voltage sources)
            continue; 
        }

        n = ncp.n;
        extraNodes = ncp.extraNodes;

        // nodes are already checked not to be 0 on creation of ncp
        conductance = ncp.DIV(n);
        A(n-1, n-1) += conductance;
        for(int en : extraNodes){
            conductance = ncp.DIV(en);
            A(n-1, en-1) += conductance;
        }
    }

    // same as linear for VS
    vector<int> nodes;
    for (int i{}; i < voltageSources.size(); i++)
    {
        const auto &vs = voltageSources.at(i);

        //slightly different for ideal opamp
        if(typeid(*vs) == typeid(OpAmp)){
            nodes = vs->getNodes();
            int node1 = nodes[0]; //Nin+
            int node2 = nodes[1]; //Nin-
            int node3 = nodes[2]; //Nout

            if (node1 != 0)
            {
                A(highestNodeNumber + i, node1 - 1) = 1;
            }

            if (node2 != 0)
            {
                A(highestNodeNumber + i, node2 - 1) = -1;
            }

            if (node3 != 0)
            {
                A(node3 - 1, highestNodeNumber + i) = 1;
            }

            continue; //Rest doesn't apply to ideal opamps
        }

        nodes = vs->getNodes();
        const int node1 = nodes[0];
        const int node2 = nodes[1];

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

        // need to add additional values when controlled sources
        if(typeid(*vs) == typeid(VoltageControlledVoltageSource)){
            float gain = vs->getGain();
            int nodeC1 = nodes[2];
            int nodeC2 = nodes[3];

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
                int controllingVsIndex = nodes[2]; //stored as 3rd node during setup

                A(highestNodeNumber + i, highestNodeNumber + controllingVsIndex) -= gain;
        }
    }

    //Important that the following loop comes after all normal voltage sources have already been added!
    if(isDc){
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
                i++;
            }
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
            int node1 = nodes[0];
            int node2 = nodes[1];

            float gain = cs->getGain();
            int controllingVsIndex = nodes[2]; //stored as 3rd node during setup

            if(node1 != 0){
                A(node1 - 1, highestNodeNumber + controllingVsIndex) += gain;
            }
            if(node2 != 0){
                A(node2 - 1, highestNodeNumber + controllingVsIndex) -= gain;
            }
        }
    }
}

MatrixXd Circuit::getA() const
{
    return A;
}

void Circuit::computeA_inv(){
    A_inv = A.inverse();
}

MatrixXd Circuit::getA_inv() const{
    return A_inv;
}

// setupB definition
void Circuit::adjustB(bool isDc)
{
    int extraZeroVSNumber = isDc ? inductorNumber : 0;
    
    b = VectorXd::Zero(highestNodeNumber + voltageSources.size() + extraZeroVSNumber);
    
    //adding currents
    for (const auto &cs : currentSources)
    {
        if(isDc && (typeid(*cs) == typeid(Capacitor) || typeid(*cs) == typeid(Inductor))){
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

        if(typeid(*vs) == typeid(VoltageControlledVoltageSource) || typeid(*vs) == typeid(CurrentControlledVoltageSource) || typeid(*vs) == typeid(OpAmp)){
            continue;
        }else{ // normal/independent voltage sources
            b(i) = vs->getVoltage();
        }
    }
}

void Circuit::nonLinearB(bool isDc, float alpha){
    int extraZeroVSNumber = isDc ? inductorNumber : 0;

    b = VectorXd::Zero(highestNodeNumber + voltageSources.size() + extraZeroVSNumber);

    int n, n1, n2;
    vector<int> nodes, extraNodes;

    double current;
    // setup currents from non voltage source components
    for(const nodeCompPair ncp : nodalFunctions){
         if(isDc && (typeid(*ncp.comp) == typeid(Capacitor) || typeid(*ncp.comp) == typeid(Inductor))){
            continue; // Open/short circuit at DC => Don't contribute currents
        }

        n = ncp.n;
        extraNodes = ncp.extraNodes;
        current = ncp.IV();

        //for source stepping (multiply all sources by alpha)
        if(isDc && typeid(*ncp.comp) == typeid(CurrentSource)){
            b(n-1) -= current*alpha;
        }else{
            b(n-1) -= current;
        }
    }

    //adding voltages
    for (int i{highestNodeNumber}, j{}; i < highestNodeNumber + voltageSources.size(); i++, j++)
    {
        const auto &vs = voltageSources.at(j);

        if(typeid(*vs) == typeid(VoltageControlledVoltageSource) || typeid(*vs) == typeid(CurrentControlledVoltageSource) || typeid(*vs) == typeid(OpAmp)){
            continue;
        }else{ // normal/independent voltage sources
            b(i) += vs->getVoltage()*alpha;
        }        
    }
};

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

void Circuit::computeNLX(float gamma){
    x = A_inv * b;
}

void Circuit::setX(VectorXd newX){
    x = newX;
}

VectorXd Circuit::getX() const{
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

void Circuit::setupCurrentControlledSources(Circuit &c){
    for(const auto &source : c.getCurrentSourcesRef()){
        if(typeid(*source) == typeid(CurrentControlledCurrentSource)){
            int vsIdx = getVoltageSourceIndexByName(source->getVsName(), c.getVoltageSourcesRef());
            source->appendToNodes(vsIdx);
        }
    }
    for(const auto &source : c.getVoltageSourcesRef()){
        if(typeid(*source) == typeid(CurrentControlledVoltageSource)){
            int vsIdx = getVoltageSourceIndexByName(source->getVsName(), c.getVoltageSourcesRef());
            source->appendToNodes(vsIdx);
        }
    }
}

int Circuit::getVoltageSourceIndexByName(string vsName, vector<Component*>& voltageSources) const{
    for(int i{}; i<voltageSources.size(); i++){
        if(voltageSources.at(i)->getName() == vsName){
            return i;
        }
    }
}
