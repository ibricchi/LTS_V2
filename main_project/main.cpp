#include <iostream>
#include <vector>
#include <fstream>

#include <circuit/circuit.hpp>
#include <input/input.hpp>
#include <output/output.hpp>

using namespace std;

void seriesResistors(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesResistors" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(int i{1}; i<count; i++){
        buffer << "R" << i << " n" << i << " n" << i+1 << " 1k" <<endl;
    }
    buffer << "R" << count << " n" << count << " 0 1k" <<endl;
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

void seriesAcDiode(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesAcDiode" << endl;
    buffer << ".model D D" << endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i < count; i++){
        buffer << "D" << i << " n" << i << " n" << i+1 << " D" <<endl;
    }
    buffer << "R" << count << " n" << count << " 0 1k" <<endl;
    buffer << ".tran 0.001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

int main(int argc, char **argv){
    // commands for efficiency testing
    int compCount = 1; // must be 1 at least
    if(argc == 2){
        compCount = stoi(argv[1]);
    }

    stringstream buffer{};
    
    // resistors test
    seriesResistors(buffer, compCount);

    // diode test
    // seriesAcDiode(buffer, compCount);

    //get optional input arguments
    string outputFileName = "out.csv";
    // setup circuit
    Circuit c{};
    
    // stringstream buffer;
    // testCircuit(buffer);

     readSpice(c, buffer);
    // readSpice(c, cin); //use this if want to read from txt file

    // run simulation
    outputCSV(c, outputFileName);
}
