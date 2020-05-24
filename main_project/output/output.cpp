#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <circuit/circuit.hpp>
#include <component/component.hpp>
#include <component/resistor.hpp>
#include <component/capacitor.hpp>
#include <component/inductor.hpp>
#include <component/voltageControlledVoltageSource.hpp>
#include <component/currentControlledVoltageSource.hpp>
#include <component/voltageControlledCurrentSource.hpp>
#include <component/currentControlledCurrentSource.hpp>
#include <component/opamp.hpp>

#include "linearAnalysis.hpp"
#include "nonLinearAnalysis.hpp"

#include "output.hpp"

void outputCSV(Circuit& c, string outputFileName){
    //get references to the components stored inside the circuit
    vector<Component*> voltageSources = c.getVoltageSourcesRef();
    vector<Component*> currentSources = c.getCurrentSourcesRef();
    vector<Component*> conductanceSources = c.getConductanceSourcesRef();

    //get simulation parameters from circuit
    float timeStep = c.getMaxTimeStep(); //Change once we have implemented a dynamic timestep
    float simulationTime = c.getSimulationTime();
    // float tStep = c.getTStep(); //printing increment for csv file
    // float maxTimeStep = c.getMaxTimeStep(); //used for dynamic timestep

    //setup csv file
    ofstream outputFile;
    outputFile.open(outputFileName);

    //add headers to csv file
    //display time, node voltages, current through components
    outputFile << "time";
    int highestNodeNumber = c.getHighestNodeNumber();
    for(int i{1}; i<=highestNodeNumber; i++){
        outputFile << ",v_" + to_string(i);
    }
    //conductance sources
    for(const auto &gs : conductanceSources){
        if(typeid(*gs) != typeid(Resistor)){
            continue; //don't want to display current through the companion model's resistor
        }
        outputFile << ",i_" + gs->getName();
    }
    //voltage sources
    for(const auto &vs : voltageSources){
        outputFile << ",i_" + vs->getName();
    }
    //current sources + other components
    for(const auto &cs : currentSources){
        outputFile << ",i_" + cs->getName();
    }
    outputFile << "\n";
    
    runAnalysis(c, outputFile, timeStep, simulationTime);
    
    outputFile.close();
}

void runAnalysis(Circuit& c, ofstream& outputFile, float timeStep, float simulationTime){
    string outLine{};
    if(!c.hasNonLinearComponents()){
        linearSetup(c); //compute A, b, A_inv, xMeaning
        for(float t = 0; t<=simulationTime; t += timeStep){
            outLine = runLinearTransience(c, t); 
            outputFile << outLine << endl;
        }
    }else{
        nonLinearSetup(c);
        for(float t = 0; t<=simulationTime; t += timeStep){// could replace with a while loop if we ever do dynamic time steps
            outLine = runNonLinearTransience(c, t); 
            outputFile << outLine << endl;
        }
    }
}