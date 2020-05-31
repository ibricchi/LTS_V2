#include <iostream>
#include <vector>
#include <fstream>

#include <circuit/circuit.hpp>
#include <input/input.hpp>
#include <output/output.hpp>

// for debugging only
#include <circuit/circuit.cpp>

#include <component/bjt.cpp>
#include <component/capacitor.cpp>
#include <component/component.cpp>
#include <component/currentControlledCurrentSource.cpp>
#include <component/currentControlledSource.cpp>
#include <component/currentControlledVoltageSource.cpp>
#include <component/currentSource.cpp>
#include <component/diode.cpp>
#include <component/exampleInputDiode.hpp>
#include <component/inductor.cpp>
#include <component/mosfet.cpp>
#include <component/opamp.cpp>
#include <component/resistor.cpp>
#include <component/voltageControlledCurrentSource.cpp>
#include <component/voltageControlledSource.cpp>
#include <component/voltageControlledVoltageSource.cpp>
#include <component/voltageSource.cpp>
#include <component/waveform.cpp>

#include <input/input.cpp>

#include <output/linearAnalysis.cpp>
#include <output/nonLinearAnalysis.cpp>
#include <output/output.cpp>

using namespace std;

int main(int argc, char **argv){
    //get optional input arguments
    string outputFileName = "out.csv";

    // setup circuit
    Circuit c{};
    
    // stringstream buffer;
    // testCircuit3(buffer);
    // readSpice(c, buffer);
    
    readSpice(c, cin); //use this if want to read from txt file

    // run simulation
    outputCSV(c, outputFileName);
}