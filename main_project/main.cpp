#include <iostream>
#include <vector>
#include <fstream>

#include <circuit/circuit.hpp>
#include <input/input.hpp>
#include <output/output.hpp>

using namespace std;

// for debugging only
void testCircuit1(stringstream& buffer){
    buffer << "ExampleCircuit1" << endl;
    buffer << "V1 2 1 SIN(0 10 10)" << endl;
    buffer << "R1 3 2 2000" << endl;
    buffer << "L1 1 0 .1" << endl;
    buffer << "R2 3 0 5000" << endl;
    buffer << "I1 4 3 .5" << endl;
    buffer << "R3 4 0 10000" << endl;
    buffer << "C1 4 0 .000001" << endl;
}
void testCircuit2(stringstream& buffer){
    buffer << "ExampleCircuit2" << endl;
    buffer << "V1 1 0 SIN(0 10 10)" << endl;
    buffer << "R1 2 0 1000" << endl;
    buffer << "L1 1 2 .1" << endl;
}
void testCircuit3(stringstream& buffer){
    buffer << "ExampleCircuit3" << endl;
    buffer << "V1 1 0 SIN(0 18 10)" << endl;
    buffer << "R1 1 2 4k" << endl;
    buffer << "R2 2 3 1k" << endl;
    buffer << "D1 3 4" << endl;
    buffer << "V2 4 0 3" << endl;
    buffer << "R3 2 5 4k" << endl;
    buffer << "C1 5 0 1n" <<endl;
}
void testCircuit4(stringstream& buffer){
    buffer << "ExampleCircuit4" << endl;
    buffer << "V1 1 0 SIN(2 10 10)" << endl;
    buffer << "R2 1 0 1" << endl;
    buffer << "R1 1 2 .5" << endl;
    buffer << "D1 2 0" <<endl;
}
void testCircuit(stringstream& buffer){
    buffer << "ExampleCircuit5" << endl;
    buffer << "R1 1 2 1k" << endl;
    buffer << "R2 2 0 1k" << endl;
    buffer << "H1 3 2 V1 2" <<endl;
    buffer << "R3 3 0 1k" <<endl;
    buffer << "V1 1 0 12" << endl;
}

int main(int argc, char **argv){
    //get optional input arguments
    string outputFileName = "out.csv";
    double timeStep = 0.00001; //seconds
    double simulationTime = 0.2; //seconds
    if(argc > 1){
        outputFileName = argv[1];
    }
    if(argc > 2){
        timeStep = (double)atof(argv[2]);
    }
    if(argc > 3){
        simulationTime = (double)atof(argv[3]);
    }

    // setup circuit
    Circuit c{};
    setupBasic(c, timeStep);

    stringstream buffer;
    testCircuit(buffer);

    readSpice(c, buffer);

    // run simulation
    outputCSV(c, outputFileName, timeStep, simulationTime);
}


