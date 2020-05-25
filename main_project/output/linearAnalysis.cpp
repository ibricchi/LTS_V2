#include <vector>
#include <string>
#include <circuit/circuit.hpp>
#include <component/component.hpp>
#include <component/capacitor.hpp>
#include <component/inductor.hpp>
#include <component/currentSource.hpp>

#include <iostream> //testing only

#include "linearAnalysis.hpp"

void linearDCSetup(Circuit& c){
    c.setupA(true);
    c.adjustB(true);
    c.computeA_inv();
}

void linearSetup(Circuit& c){
    c.setupA();
    c.adjustB();
    c.computeA_inv();
    c.setupXMeaning();
}

string runLinearTransience(Circuit& c, float t){
    //get references to the components stored inside the circuit
    vector<Component*> voltageSources = c.getVoltageSourcesRef();
    vector<Component*> currentSources = c.getCurrentSourcesRef();
    vector<Component*> conductanceSources = c.getConductanceSourcesRef();
    vector<Component*> vcUpdatables = c.getVCUpdatablesRef();
    vector<Component*> timeUpdatables = c.getTimeUpdatablesRef();
    int highestNodeNumber = c.getHighestNodeNumber();

    string outLine{};
    //compute x for the current timestep
    c.computeX();
    VectorXd x = c.getX();

    //output current time 
    c.setCurrentTime(t);
    outLine += to_string(t);

    //output node voltages
    for(int i{}; i<highestNodeNumber; i++){
        outLine += "," + to_string(x(i));
    }

    //output current through resistors
    vector<int> nodes{};
    float voltage{}, v1{}, v2{};
    float current{};
    for(const auto &gs : conductanceSources){
        if(typeid(*gs) == typeid(Inductor) || typeid(*gs) == typeid(Capacitor)){
            continue; //don't want to display current through the companion model's resistor
        }
        
        nodes = gs->getNodes();

        v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1);
        v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1);
        voltage = v1 - v2;

        current = voltage * gs->getConductance();

        outLine += "," + to_string(current);
    }

    //output current through voltage sources
    for(int i{}; i<voltageSources.size(); i++){
        current = x(highestNodeNumber+i);

        outLine += "," + to_string(current);
    }

    //output current through current sources/other components
    for(const auto &cs : currentSources){
        if((typeid(*cs) == typeid(Capacitor)) || typeid(*cs) == typeid(Inductor)){ //component = inductor/capacitor
         	nodes = cs->getNodes();
		    v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1);
        	v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1);  
		
		outLine += "," + to_string(cs->getTotalCurrent(v1-v2));
        }else if(typeid(*cs) == typeid(CurrentSource)){
            outLine += "," + to_string(cs->getCurrent());
        }else{
            outLine += ",NotImplemented";
        }
    }

    
    // update components before next calculation of b
    for(const auto &comp : timeUpdatables){
        comp->updateVals(t+c.getTimeStep());
    }
    
    //update components based on current voltage/current
    float currentVoltage{}, currentCurrent{};
    for(const auto &up : vcUpdatables){
        nodes = up->getNodes();

        v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1);
        v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1);
        currentVoltage = v1 - v2;

        up->updateVals(currentVoltage, 0, 1);


        cout <<endl<<endl;
        cout << "time: " << t <<endl;
        cout << "voltage: " << currentVoltage <<endl;
        cout << "conductance: " << up->getConductance() <<endl;
        cout << "current: " << up->getCurrent() <<endl;
        cout << "total current: " << up->getTotalCurrent(currentVoltage) <<endl;
        cout <<endl<<endl;
    }

    //update b for calculations at next timestep
    c.adjustB();

    return outLine;
}
