#include <iostream>
#include <vector>
#include <fstream>

#include <circuit/circuit.hpp>
#include <input/input.hpp>
#include <output/output.hpp>

using namespace std;

// for debugging only
void testCircuit15(stringstream& buffer){
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
void testCircuit3(stringstream& buffer){
    buffer << "ExampleCircuit1" << endl;
    buffer << "V1 1 0 SIN(0 1 10)" << endl;
    buffer << "Q1 2 1 0" << endl;
    buffer << "R1 2 3 1k" << endl;
    buffer << "V2 3 0 1" << endl;
}
<<<<<<< HEAD


void testCircuit7(stringstream& buffer){
    buffer << "ExampleCircuit1" << endl;
    buffer << "V1 1 0 SIN(0 1 10)" << endl;
    buffer << "V1 2 0 7" << endl;
    buffer << "R1 2 3 1" << endl;
    buffer << "Q1 3 1 0" << endl;
}
void testCircuit(stringstream& buffer){
    buffer << "ExampleCircuit8" << endl;
    buffer << "I1 N1 0 PWL(0 10 .1 10 .101 0)" <<endl;
    buffer << "R1 N1 0 1" <<endl;
    buffer << "R2 n1 n2 1k" <<endl;
    buffer << "R3 n2 0 100k" <<endl;
    buffer << "C1 n2 0 0.00001" <<endl;
    buffer << ".tran 0.0001 0.2 0 0.0001s" <<endl;
    buffer << ".end" <<endl;
}
void testCircuit9(stringstream& buffer){
    buffer << "ExampleCircuit9" << endl;
    buffer << "V1 N1 0 20" << endl;
    buffer << "R1 N2 N3 2k" << endl;
    buffer << "R2 N2 0 1k" <<endl;
    buffer << "X1 N1 N2 N3 IDEAL" << endl;
    buffer << ".tran 0.1 0.5" <<endl;
    buffer << ".end" <<endl;
}

int main(int argc, char **argv){
    //get optional input arguments
    string outputFileName = "out.csv";
    float timeStep = 0.0001; //seconds
    float simulationTime = 0.001; //seconds
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
    
    stringstream buffer;
    testCircuit(buffer);

    readSpice(c, buffer);
    // readSpice(c, cin); //use this if want to read from txt file

    // run simulation
    outputCSV(c, outputFileName);
}
