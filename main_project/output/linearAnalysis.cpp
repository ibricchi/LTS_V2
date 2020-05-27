#include <vector>
#include <string>
#include <circuit/circuit.hpp>
#include <component/component.hpp>
#include <component/capacitor.hpp>
#include <component/inductor.hpp>
#include <component/currentSource.hpp>

#include "linearAnalysis.hpp"

//these don't change during simulation with linear components
void linearSetup(Circuit& c){
    c.setupCurrentControlledSources(c); //add idx of the controlling voltage source (must come prior to setting up A)
    c.setupA();
    c.adjustB();
    c.computeA_inv();
    c.computeX();
    c.updateNodalVoltages();
    // c.setupXMeaning(); //not really using this right now
}

string runLinearTransience(Circuit& c, float t){
    //get references to the components stored inside the circuit
    vector<Component*> components = c.getComponentsRef();
    vector<Component*> voltageSources = c.getVoltageSourcesRef();
    vector<Component*> currentSources = c.getCurrentSourcesRef();
    vector<Component*> conductanceSources = c.getConductanceSourcesRef();
    vector<Component*> vcUpdatables = c.getVCUpdatablesRef();
    vector<Component*> timeUpdatables = c.getTimeUpdatablesRef();
    int highestNodeNumber = c.getHighestNodeNumber();

    //forms a row in the csv file
    string outLine{};

    //compute x for the current timestep
    c.computeX();
    VectorXd x = c.getX();

    //output current time 
    c.setCurrentTime(t); // do we need this?
    outLine += to_string(t);

    //output node voltages
    for(int i{}; i<highestNodeNumber; i++){
        outLine += "," + to_string(x(i));
    }

    //output component currents
    for(const auto &comp : components){
        outLine += "," + to_string(comp->getTotalCurrent(x));
    }
    
    // update components before next calculation of b
    for(const auto &comp : timeUpdatables){
        comp->updateVals(t+c.getTimeStep());
    }
    
    //update components based on current voltage/current
    float currentVoltage{}, currentCurrent{};
    for(const auto &up : vcUpdatables){
        auto nodes = up->getNodes();

        float v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1);
        float v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1);
        currentVoltage = v1 - v2;

        up->updateVals(currentVoltage, 0, 1);
    }

    //update b for calculations at next timestep
    c.adjustB();

    return outLine;
}
