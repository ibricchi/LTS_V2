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
    c.setupCurrentControlledSources(c);//
    c.setupA(true);
    c.adjustB(true);
    c.computeA_inv();
    c.computeX();//
    c.updateNodalVoltages();//
}

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
    vector<int> nodes{};
    float voltage{}, v1{}, v2{};
    float current{};
    c.updateNodalVoltages();
	//output component currents    
	for(const auto &comp : components){
        	outLine += "," + to_string(comp->getTotalCurrent(x, highestNodeNumber));

    }
    
    // update components before next calculation of b
    for(const auto &comp : timeUpdatables){
        comp->updateVals(t+c.getTimeStep());
    }
    
    //update components based on current voltage/current
    float currentVoltage{}, currentCurrent{};
    int numberOfInductors = c.getInductorNumber();
    int whichInductor = 0; //Starts count from zero, so if whichInductor == 3, it's on the 4th inductor, variable shows which inductor is being referred to in the following for    
    for(const auto &up : vcUpdatables){
        nodes = up->getNodes();

        float v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1);
        float v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1);
        currentVoltage = v1 - v2;
        
        //These next lines initialise the compCurrent values of capacitors and inductors based on DC bias values of voltage and current
		if(t==-1){
			if(typeid(*up)==typeid(Capacitor)){
				up->initCompCurrent(currentVoltage);
			}else if(typeid(*up)==typeid(Inductor)){
				up->initCompCurrent(c.getX()[c.getX().size()-numberOfInductors+whichInductor]);
				whichInductor++;
			}	
		}
       
        // cout <<endl<<endl;
        // cout << "time: " << t <<endl;
        // cout << "voltage: " << currentVoltage <<endl;
        // cout << "conductance: " << up->getConductance() <<endl;
        // cout << "current: " << up->getCurrent() <<endl;
        // //cout << "total current: " << up->getTotalCurrent(currentVoltage) <<endl;
        // cout <<endl<<endl;

    //Must be run after getTotalCurrent
	 up->updateVals(currentVoltage, 0, 1);
    }

    //update b for calculations at next timestep
    c.adjustB();

    return outLine;
}
