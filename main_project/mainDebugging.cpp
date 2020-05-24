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
#include <component/opamp.cpp>

#include <input/input.cpp>

#include <output/output.cpp>
#include <output/linearAnalysis.cpp>
#include <output/nonLinearAnalysis.cpp>

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