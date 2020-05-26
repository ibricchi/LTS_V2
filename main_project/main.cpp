#include <iostream>
#include <vector>
#include <fstream>

#include <circuit/circuit.hpp>
#include <input/input.hpp>
#include <output/output.hpp>

using namespace std;

int main(int argc, char **argv){
    //get optional input arguments
    string outputFileName = "out.csv";
    if(argc > 1){
        outputFileName = argv[1];
    }

    // setup circuit
    Circuit c{};
    
    // stringstream buffer;
    // testCircuit3(buffer);

    // readSpice(c, buffer);
    readSpice(c, cin); //use this if want to read from txt file

    // run simulation
    outputCSV(c, outputFileName);
}