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
#include <component/currentControlledSource.cpp>
#include <component/voltageControlledSource.cpp>
#include <component/currentControlledVoltageSource.cpp>
#include <component/currentControlledCurrentSource.cpp>
#include <component/voltageControlledVoltageSource.cpp>
#include <component/voltageControlledCurrentSource.cpp>
#include <component/diode.cpp>
#include <component/bjt.cpp>

#include <input/input.cpp>

#include <output/output.cpp>
#include <output/linearAnalysis.cpp>
#include <output/nonLinearAnalysis.cpp>

using namespace std;

void testCircuit(stringstream& buffer){
    buffer << "ExampleCircuit1" << endl;
    buffer << "R1 1 3 4.7k" << endl;
    buffer << "R2 3 0 1k" << endl;
    buffer << "R3 1 2 4.7k" << endl;
    buffer << "V1 3 0 sin(0 0.1 10)" << endl;
    buffer << "V2 1 0 5" << endl;
    buffer << "Q1 2 3 6" << endl;
    buffer << "R4 1 0 1k" << endl;
}

int main(){
    //get optional input arguments
    string outputFileName = "out.csv";
    float timeStep = 0.0001; //seconds
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
