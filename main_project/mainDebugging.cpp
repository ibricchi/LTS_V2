#include <iostream>
#include <vector>
#include <fstream>

#include <circuit/circuit.hpp>
#include <input/input.hpp>
#include <output/output.hpp>

// for debugging only
#include <circuit/circuit.cpp>

#include <component/component.cpp>
#include <component/resistor.cpp>
#include <component/capacitor.cpp>
#include <component/inductor.cpp>
#include <component/voltageSource.cpp>
#include <component/currentSource.cpp>
#include <component/waveform.cpp>
#include <component/diode.cpp>
#include <component/bjt.cpp>

#include <input/input.cpp>

#include <output/output.cpp>
#include <output/linearAnalysis.cpp>
#include <output/nonLinearAnalysis.cpp>

using namespace std;

// for debugging only
void testCircuit(stringstream& buffer){
    buffer << "ExampleCircuit1" << endl;
    buffer << "V1 1 0 SIN(0 1 10)" << endl;
    buffer << "R2 1 2 10000" << endl;
    buffer << "R1 3 4 10" << endl;
    buffer << "V1 4 0 5" << endl;
    buffer << "R3 3 4 5000" << endl;
}

int main(){
    //get optional input arguments
    string outputFileName = "out.csv";
    float timeStep = 0.001; //seconds
    float simulationTime = 1; //seconds
    
    // setup circuit
    Circuit c{};
    setupBasic(c, timeStep);

    stringstream buffer;
    testCircuit(buffer);

    readSpice(c, buffer);

    // run simulation
    outputCSV(c, outputFileName, timeStep, simulationTime);
}
