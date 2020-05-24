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
    buffer << "Q1 2 1 0" << endl;
    buffer << "D1 2 3" << endl;
    buffer << "R1 2 0 1k" << endl;
    buffer << "R2 3 4 10k" << endl;
    buffer << "V1 4 0 sin(0 1 10)" << endl;
}
void testCircuit1(stringstream& buffer){
    buffer << "ExampleCircuit1" << endl;
    buffer << "V1 1 0 SIN(0 1 10)" << endl;
    buffer << "V1 2 0 7" << endl;
    buffer << "R1 2 3 1" << endl;
    buffer << "Q1 3 1 0" << endl;
}
void testCircuit2(stringstream& buffer){
    buffer << "ExampleCircuit1" << endl;
    buffer << "R1 1 3 4.7k" << endl;
    buffer << "R2 3 0 1k" << endl;
    buffer << "R3 1 2 4.7k" << endl;
    buffer << "V1 3 0 sin(0 0.1 10)" << endl;
    buffer << "V2 1 0 5" << endl;
    buffer << "Q1 2 3 4" << endl;
    buffer << "R4 1 0 1k" << endl;
}

int main(int argc, char **argv){
    //get optional input arguments
    string outputFileName = "out.csv";
    float timeStep = 0.0001; //seconds
    float simulationTime = 1; //seconds
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
    testCircuit2(buffer);

    readSpice(c, buffer);
    // readSpice(c, cin); //use this if want to read from txt file

    // run simulation
    outputCSV(c, outputFileName, timeStep, simulationTime);
}