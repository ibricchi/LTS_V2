#include <iostream>
#include <vector>
#include <fstream>

#include <circuit/circuit.hpp>
#include <input/input.hpp>
#include <output/output.hpp>

using namespace std;

// for debugging only
void testCircuit(stringstream& buffer){
    buffer << "ExampleCircuit1" << endl;
    buffer << "V1 1 0 SIN(0 1 10)" << endl;
    buffer << "R2 1 2 10000" << endl;
    buffer << "R1 3 4 10" << endl;
    buffer << "V1 4 0 5" << endl;
    buffer << "Q1 3 2 0" << endl;
}

int main(int argc, char **argv){
    //get optional input arguments
    string outputFileName = "out.csv";
    float timeStep = 0.0001; //seconds
    float simulationTime = 0.5; //seconds
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
    // Circuit c{};
    // setupBasic(c, timeStep);
    // readSpice(c, cin);

    // setup circuit
    Circuit c{};
    setupBasic(c, timeStep);
    stringstream buffer;
    testCircuit(buffer);

    readSpice(c, buffer);

    // run simulation
    outputCSV(c, outputFileName, timeStep, simulationTime);
}


