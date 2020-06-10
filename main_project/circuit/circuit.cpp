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

// basic circuit contructor with default parameters
Circuit::Circuit()
{
    // all times in seconds
    currentTime = 0;
    timeStep = 0.001;
    //prevTime = -timeStep;
    highestNodeNumber = 0;
    inductorNumber = 0;
    hasNonLinear = false;
}

// dealocate raw pointers
// all components are dynamically created using new, so must be deleted manually
Circuit::~Circuit()
{
    for(auto comp : components){
        delete comp;
    }
}

// simple getters and setters for circuit parameters
string Circuit::getTitle() const{
    return title;
}
void Circuit::setTitle(string _title){
    title = _title;
}


vector<float>& Circuit::getTimePoints(){
return timePoints;
}
void Circuit:: setTimePoints(){
	for(auto src : voltageSources){ // looks through all voltage sources
		if(typeid(*src) == typeid(VoltageSource)){ // checks if the type is an actuall votlage source
			auto res = src->getTimePoints(); // get's time points
			timePoints.insert(timePoints.end(),res.begin(),res.end()); // adds timep oints to circuit vector
		}	
	}
	for(auto src : currentSources){ // repeats process for current sources
		if(typeid(*src) == typeid(CurrentSource)){	
			auto res = src->getTimePoints();		
			timePoints.insert(timePoints.end(),res.begin(),res.end());
		}	
	}
	sort(timePoints.begin(),timePoints.end()); // sorts list by time, only called once at the beggining of simulation so time complexity is fine
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

double Circuit::getPrevTime() const{
    return prevTime;
}

void Circuit::setPrevTime(double _prevTime){
    prevTime = _prevTime;
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

double Circuit::getMinPNConductance() const{
    return minPNConductance;
}
void Circuit::setMinPNConductance(double con){
    minPNConductance = con;
}

int Circuit::getMaxNewtonRaphsonCount() const{
    return maxNewtonRaphsonCount;
}
void Circuit::setMaxNewtonRaphsonCount(int count){
    maxNewtonRaphsonCount = count;
}

float Circuit::getAbstol() const{
    return abstol;
}
void Circuit::setAbstol(float _abstol){
    abstol = _abstol;
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

// setup for non linear circuits
void Circuit::nlSetup(bool isDc){
    int nvc = nonVoltageSources.size(); // get's non voltage source size
    int vsc = voltageSources.size(); // get's voltage sources size
    int extraZeroVSNumber = isDc ? inductorNumber : 0; // if running DC bias analyis also get's inductor count

    x = VectorXd::Zero(highestNodeNumber + vsc + extraZeroVSNumber); // setup x vector to appropriate size with initial guess of 0 at all points

    // sets up nodalFunctions vector
    // creates an instance of nodeCompPair for each non ground node of a component
    vector<int> nodes;
    vector<int> extraNodes;
    for(Component* comp : nonVoltageSources){ // loops trhough all non voltage sources
        nodes = comp->getNodes(); // get's components nodes
        for(int n1 : nodes){ // loops through nodes, n1 is the main node being considered
            extraNodes.resize(0); // resets extraNodes size
            if(n1 == 0) continue; // if node is ground skip
            for(int n2 : nodes){ // loops trhough all nodes
                if(n1 == n2 || n2 == 0) continue; // if node is ground or main node skip
                extraNodes.push_back(n2); // otherwise add node to extra nodes
            }
            nodalFunctions.push_back(nodeCompPair{n1, extraNodes, comp}); // add node component pair to nodalFUnctions vector
        }            
    }
}

// setupA for linear circuits
void Circuit::setupA(bool isDc)
{
    int extraZeroVSNumber = isDc ? inductorNumber : 0; // if dc-bias is being calculated get indcutror count (they are being modeled as voltage sources)
    // setup a to the appropriate size and initialize all values to 0
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

        // for each node add conductance to appropriate part of circuit
        for (int i = 0; i < nodes.size(); i++)
        {
            if (nodes[i] == 0){ // if node is 0 skip node
                continue;
            }
            A(nodes[i] - 1, nodes[i] - 1) += conductance; // adds conductance to main position in diagpnal
            for (int j = 0; j < nodes.size(); j++)
            {
                if (i == j || nodes[j] == 0)
                    continue;
                A(nodes[i] - 1, nodes[j] - 1) -= conductance; // subtracts conducance from all other appropriate locations in row
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

            // places 1's and -1's in appropreate location in conductance matrix
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

        // normal voltage sources only have 2 nodes
        nodes = vs->getNodes();
        int node1 = nodes.at(0);
        int node2 = nodes.at(1);
        // sets approprtate 1's and -1's in conductance matrix
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
    //Sets up extra DC voltage sources created by inductors during DC analysis
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
            // these act like conductance sources through two nodes which do not include the main node
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

// setting up non linear conducntance matrix
void Circuit::nonLinearA(bool isDc){
    int extraZeroVSNumber = isDc ? inductorNumber : 0; // if DC bias anlysis inductors count is needed as they are treated like voltage sources

    // set's up conductnace matrix to appropriate size with 0's as all values
    A = MatrixXd::Zero(highestNodeNumber + voltageSources.size() + extraZeroVSNumber, highestNodeNumber + voltageSources.size() + extraZeroVSNumber);

    // setup currents from non voltage source components
    int n{};
    vector<int> extraNodes{};
    double conductance{};
    for(const nodeCompPair ncp : nodalFunctions){ // loops through all node component pairs
        //Replace capacitors/inductors with open circuit for DC operating point analysis
        if(isDc && (typeid(*ncp.comp) == typeid(Capacitor) || typeid(*ncp.comp) == typeid(Inductor))){
            //Capacitor is open circuit at DC => don't contribute conductance
            //Inductor is short circuit at DC => equivalent to zero voltage source (added after all normal voltage sources)
            continue; 
        }

        n = ncp.n; // get's main node of node component pair
        extraNodes = ncp.extraNodes; // get's the remaining non 0 nodes of node component pairs

        // sets up conducntance matrix appropriately
        // nodes are already checked not to be 0 on creation of ncp
        conductance = ncp.DIV(n); // runs DIV in relation to main component
        A(n-1, n-1) += conductance;
        for(int en : extraNodes){ // runs DIV in relation to remainin components
            conductance = ncp.DIV(en);
            A(n-1, en-1) += conductance;
        }
        // IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
        // cout << A.format(CleanFmt) << endl << endl;
    }

    // After this point linear and non linear creation of A matrix is identical
    
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

// simple getter for conductance matrix
MatrixXd Circuit::getA() const
{
    return A;
}
// calculates A invers
void Circuit::computeA_inv(){
    A_inv = A.inverse();
}
MatrixXd Circuit::getA_inv() const{
    return A_inv;
}

// setupB definition
void Circuit::adjustB(bool isDc)
{
    int extraZeroVSNumber = isDc ? inductorNumber : 0; // if DC bias anlysis inductors count is needed as they are treated like voltage sources
    
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
    // if DC bias anlysis inductors count is needed as they are treated like voltage sources
    int extraZeroVSNumber = isDc ? inductorNumber : 0;

    // setup b matrix to appropreate size with all values at 0
    b = VectorXd::Zero(highestNodeNumber + voltageSources.size() + extraZeroVSNumber);

    int n, n1, n2;
    vector<int> nodes, extraNodes;

    double current;
    // setup currents from non voltage source components
    for(const nodeCompPair ncp : nodalFunctions){ // loops trhough node component pairs

        // exclusion during DC biasing
         if(isDc && (typeid(*ncp.comp) == typeid(Capacitor) || typeid(*ncp.comp) == typeid(Inductor))){
            continue; // Open/short circuit at DC => Don't contribute currents
        }

        n = ncp.n; // get main node for node component pair
        extraNodes = ncp.extraNodes; // get the extra non ground nodes for the pair
        current = ncp.IV(); // get the current source at the main node

        //for source stepping (multiply all sources by alpha)
        if(isDc && typeid(*ncp.comp) == typeid(CurrentSource)){
            b(n-1) -= current*alpha;
        }else{
            b(n-1) -= current;
        }
    }

    //adding voltages
    for (int i{highestNodeNumber}, j{}; i < highestNodeNumber + voltageSources.size(); i++, j++) // loop through voltage sources
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

// setting up the meaning at each node in x for the output
void Circuit::setupXMeaning(){
    xMeaning.clear(); // reset x meaning

    // sets up node voltage outputs
    for(int i{1}; i<=highestNodeNumber; i++){
        xMeaning.push_back("v_" + to_string(i));
    }

    // sets up voltage source current outputs
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
// calls update voltage source on all components
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
