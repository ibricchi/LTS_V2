#include <vector>
#include <string>
#include <circuit/circuit.hpp>
#include <component/component.hpp>
#include <component/capacitor.hpp>
#include <component/inductor.hpp>
#include <component/currentSource.hpp>

#include "nonlinearAnalysis.hpp"

// doesn't change during nonlinear analysis
void nonlinearSetup(Circuit& c){
    c.setupXMeaning();
}

string runNonlinearTransience(Circuit& c, float t){
    //get references to the components stored inside the circuit
    vector<Component*> voltageSources = c.getVoltageSourcesRef();
    vector<Component*> currentSources = c.getCurrentSourcesRef();
    vector<Component*> conductanceSources = c.getConductanceSourcesRef();
    vector<Component*> vcUpdatables = c.getVCUpdatablesRef();
    vector<Component*> timeUpdatables = c.getTimeUpdatablesRef();
    vector<Component*> nonLinears = c.getNonLinearsRef();

    int highestNodeNumber = c.getHighestNodeNumber();

    string outLine{};

    ///Newton-Raphson loop:

    VectorXf x;

    //variables that are used multiple times in this function
    vector<int> nodes{};
    float prevVoltage{-1.0f}, voltage{}, v1{}, v2{};
    float current{};

    //set maximum Newton-Raphson error
    float nrError = 0.001;

    bool flag;
    do{
        flag = false;

        //compute A, b for current iteration
        c.setupA();
        c.adjustB();

        //compute x for the current iteration
        c.computeA_inv();
        c.computeX();
        VectorXf x = c.getX();

        //update values for non linear components (used in next iteration)
        for(const auto comp : nonLinears){
            nodes = comp->getNodes();

            v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1);
            v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1);
            voltage = v1 - v2;

            //check if a nonlinear component has not yet converged
            if(prevVoltage != -1.0f && abs(voltage - prevVoltage) > nrError){
                flag = true; //do another iteration
            }

            comp->updateVals(voltage);

            prevVoltage = voltage;
        }
    }while(flag);


    ///output results for current timestep:

    //output current time 
    c.setCurrentTime(t);
    outLine += to_string(t);

    //output node voltages
    for(int i{}; i<highestNodeNumber; i++){
        outLine += "," + to_string(x(i));
    }

    //output current through resistors
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
        if(typeid(*cs) == typeid(CurrentSource)){ //component = currentSource
         	outLine += "," + to_string(cs->getCurrent());
        }else{ //component = everything with companion models
            nodes = cs->getNodes();
		    v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1);
        	v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1);  
		
		    outLine += "," + to_string(cs->getTotalCurrent(v1-v2));
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

        //currentCurrent = currentVoltage * up->getConductance();
        
        // IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
        // cout << A.format(CleanFmt) << endl << endl;
        // cout << b.format(CleanFmt) << endl << endl;
        // cout << x.format(CleanFmt) << endl <<endl;

        up->updateVals(currentVoltage, 0, 1);
    }

    return outLine;
}
