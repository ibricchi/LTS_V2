#include <vector>
#include <string>

#include <circuit/circuit.hpp>
#include <component/component.hpp>
#include <component/resistor.hpp>
#include <component/capacitor.hpp>
#include <component/inductor.hpp>
#include <component/currentSource.hpp>
#include <component/voltageControlledCurrentSource.hpp>
#include <component/currentControlledCurrentSource.hpp>
#include <component/diode.hpp>
#include <component/mosfet.hpp>

#include "nonlinearAnalysis.hpp"

// doesn't change during nonlinear analysis
void nonlinearSetup(Circuit& c){
    c.setupXMeaning();
}

string runNonlinearTransience(Circuit& c, double t){
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

    VectorXd x = VectorXd::Zero(highestNodeNumber + voltageSources.size());

    //variables that are used multiple times in this function
    vector<int> nodes{};
    vector<double> prevVoltages{}; //for newton_raphson convergence checking
    double voltage{}, v1{}, v2{}, v3{};
    double current{};

    //set maximum Newton-Raphson error
    double nrError = 0.0001;

    bool flag;
    do{
        flag = false;

        //compute A, b for current iteration
        c.setupA();
        c.adjustB();

        //compute x for the current iteration
        c.computeA_inv();
        c.computeX();
        x = c.getX();

        //update values for non linear components (used in next iteration)
        for(const auto comp : nonLinears){
            nodes = comp->getNodes();
            prevVoltages = comp->getPrevVoltages();

            if(typeid(*comp) == typeid(Diode)){
                v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1);
                v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1);
                voltage = v1 - v2;
                
                //check if a nonlinear component has not yet converged
                if(prevVoltages.at(0) == -1.0f || abs(voltage - prevVoltages.at(0)) > nrError){
                    flag = true; //do another iteration

                    comp->updateVals(voltage);
                }
            }else if(typeid(*comp) == typeid(Mosfet)){
                v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1); //vd
                v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1); //vg
                v3 = nodes.at(2) == 0 ? 0 : x(nodes.at(2)-1); //vs
                double vgs = v2 - v3;
                double vds = v1 - v3;

                //check if a nonlinear component has not yet converged
                if(prevVoltages.at(0) == -1.0f || (abs(vgs - prevVoltages.at(0)) > nrError && abs(vds - prevVoltages.at(1)) > nrError)){
                    flag = true; //do another iteration

                    comp->updateVals(vgs, vds);
                }
            }
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
        if(typeid(*gs) != typeid(Resistor)){
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
        }else if((typeid(*cs) == typeid(VoltageControlledCurrentSource)) || (typeid(*cs) == typeid(CurrentControlledCurrentSource))){
            outLine += ",NotImplemented"; //still need to figure out way to compute current here
        }else if(typeid(*cs) == typeid(Mosfet)){
            nodes = cs->getNodes();
		    v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1); //vd
            v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1); //vg
            v3 = nodes.at(2) == 0 ? 0 : x(nodes.at(2)-1); //vs
            double vgs = v2 - v3;
            double vds = v1 - v3;

            // outLine += "," + to_string(cs->getTotalCurrent(vgs, vds));
            outLine += ",NotImplemented";
        }else{ //component = everything with 2 terminal companion models
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
    double currentVoltage{}, currentCurrent{};
    for(const auto &up : vcUpdatables){
        nodes = up->getNodes();

        v1 = nodes.at(0) == 0 ? 0 : x(nodes.at(0)-1);
        v2 = nodes.at(1) == 0 ? 0 : x(nodes.at(1)-1);
        currentVoltage = v1 - v2;

        up->updateVals(currentVoltage, 0, 1);
    }

    return outLine;
}
