#include <iostream>
#include <vector>
#include <fstream>

#include "circuit.hpp"

#include "inputModule/input.hpp"
#include "outputModule/output.hpp"
#include "outputModule/linearAnalysis.hpp"

// for debugging only
#include "circuit.cpp"
#include "component.cpp"
#include "resistor.cpp"
#include "voltageSource.cpp"
#include "currentSource.cpp"
#include "capacitor.cpp"
#include "inductor.cpp"
#include "waveform.cpp"
#include "inputModule/input.cpp"
#include "outputModule/output.cpp"
#include "outputModule/linearAnalysis.cpp"

using namespace std;

// for debugging only
void testCircuit(stringstream& buffer){
    buffer << "ExampleCircuit1" << endl;
    buffer << "Vb 1 0 SIN(0 30 1)" << endl;
    buffer << "R1 1 2 5" << endl;
    buffer << "R2 2 0 3" << endl;
    buffer << "R3 2 0 10" << endl;
    buffer << "Is 2 0 2" << endl;
}

int main(int argc, char **argv){
    //get optional input arguments
    string outputFileName = "out.csv";
    float timeStep = 0.1; //seconds
    float simulationTime = 25; //seconds
    if(argc > 1){
        outputFileName = argv[1];
    }
    if(argc > 2){
        timeStep = (float)atof(argv[2]);
    }
    if(argc > 3){
        simulationTime = (float)atof(argv[3]);
    }
    
    // setup circuit
    Circuit c{};
    setupBasic(c, timeStep);
    // readSpice(c, cin);

    // debugging only
    stringstream buffer;
    testCircuit(buffer);
    readSpice(c, buffer);

    // run simulation
    outputCSV(c, outputFileName, timeStep, simulationTime);
}


