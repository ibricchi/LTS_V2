#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Eigen/Dense>

#include "linearAnalysis.hpp"

#include "./../circuit.hpp"
#include "./../component.hpp"

#include "./../voltageSource.hpp"
#include "./../currentSource.hpp"
#include "./../resistor.hpp"
#include "./../capacitor.hpp"
#include "./../inductor.hpp"


void outputCSV(Circuit& c, string outputFileName, float timeStep, float simulationTime){
    //get references to the components stored inside the circuit
    vector<Component*> voltageSources = c.getVoltageSourcesRef();
    vector<Component*> currentSources = c.getCurrentSourcesRef();
    vector<Component*> conductanceSources = c.getConductanceSourcesRef();

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
        outputFile << ",i_R" + gs->getName();
    }
    //voltage sources
    for(const auto &vs : voltageSources){
        outputFile << ",i_V" + vs->getName();
    }
    //current sources (do we care about outputing current through current sources?)
    for(const auto &cs : currentSources){
        outputFile << ",i_I" + cs->getName();
    }
    outputFile << "\n";
    
    runAnalysis(c, outputFile, timeStep, simulationTime);
    
    outputFile.close();
}

void runAnalysis(Circuit& c, ofstream& outputFile, float timeStep, float simulationTime){
    string outLine{};
    for(u_int t{}; t<=simulationTime; t+=timeStep){// could replace with a while loop if we ever do dinamic time steps
        outLine = "";
        if(!c.hasNonLinearComponents()){
            linearSetup(c);
            outLine = runLinearTransience(c, t); 
        }
        outputFile << outLine << endl;
    }
}