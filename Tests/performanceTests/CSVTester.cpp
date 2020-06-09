#include <iostream>
#include <vector>
#include <fstream>
#include <chrono> 

#include <circuit/circuit.hpp>
#include <input/input.hpp>
#include <output/output.hpp>

using namespace std;
using namespace std::chrono; 


//simple test circuit that is used for dynamic timestep/ dynamic simulation time test
void timestep1(stringstream& buffer){
    buffer.clear();
    buffer << "timestep1" << endl;
    buffer << "V1 n1 0 SIN(0 10 10)" << endl;
    buffer << "R1 n2 0 1k" << endl;
    buffer << "R2 n1 n2 500" << endl;
}
//more complex test circuit that is used for dynamic timestep/ dynamic simulation time test
void timestep2(stringstream& buffer){
    buffer.clear();
    buffer << "timestep2" << endl;
    buffer << "V1 n2 n1 SIN(0 10 10)" << endl;
    buffer << "R1 n3 n2 2k" << endl;
    buffer << "L1 n1 0 .1" << endl;
    buffer << "R2 n3 0 5k" << endl;
    buffer << "I1 n4 3 .5" << endl;
    buffer << "R3 n4 0 10k" << endl;
    buffer << "C1 n4 0 1u" << endl;
}


//circuit containing variable number of series resistors
void seriesResistors(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesResistors" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "R" << i << " n" << i << " n" << i+1 << " 1k" <<endl;
    }
    buffer << "R" << count << " n" << count << " 0 1k" <<endl;
}



//circuit containing variable number of series capacitors
void seriesCapacitors(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesCapacitors" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "C" << i << " n" << i << " n" << i+1 << " 1u" <<endl;
    }
    buffer << "C" << count << " n" << count << " 0 1u" <<endl;
}



//circuit containing variable number of series inductors
void seriesInductors(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesInductors" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "L" << i << " n" << i << " n" << i+1 << " 10m" <<endl;
    }
    buffer << "L" << count << " n" << count << " 0 10m" <<endl;
}



//circuit containing variable number of constant voltage sources
void seriesVS(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesVS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "V" << i << " n" << i << " n" << i+1 << " 10" <<endl;
    }
    buffer << "V" << count << " n" << count << " 0 10" <<endl;
}


//circuit containing variable number of constant current sources
void seriesCS(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesCS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "I" << i << " n" << i << " n" << i+1 << " 2" <<endl;
    }
    buffer << "I" << count << " n" << count << " 0 2" <<endl;
}


//circuit containing variable number of op-amps
//negative terminal connected with output + positive terminal connected to ground
void seriesOpAmps(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesOpAmps" <<endl;
    buffer << "V1 1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<=count; i++){
        buffer << "X" << i << " N" << i << " 0" << " N" << i << " opamp" <<endl;
    }
}

//voltage controlled voltage source
void seriesVCVS(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesVCVS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "E" << i << " n" << i << " n" << i+1 << " n" << i-1 << " n" << i << " 2" <<endl;
    }
    buffer << "E" << count << " n" << count << " 0 n" << count-1 << " n" << count << " 2" <<endl;
}

//voltage controlled current source
void seriesVCCS(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesVCCS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "G" << i << " n" << i << " n" << i+1 << " n" << i-1 << " n" << i << " 2" <<endl;
    }
    buffer << "G" << count << " n" << count << " 0 n" << count-1 << " n" << count << " 2" <<endl;
}

//current controlled voltage source
void seriesCCVS(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesCCVS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "H" << i << " n" << i << " n" << i+1 << " V1 2" <<endl;
    }
    buffer << "H" << count << " n" << count << " 0 V1 2" <<endl;
}

//current controlled current source
void seriesCCCS(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesCCCS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "F" << i << " n" << i << " n" << i+1 << " V1 2" <<endl;
    }
    buffer << "F" << count << " n" << count << " 0 V1 2" <<endl;
}

//circuit containing variable number of diodes (with AC source)
void seriesAcDiode(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesAcDiode" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "D" << i << " n" << i << " n" << i+1 << " D" <<endl;
    }
    buffer << "D" << count << " n" << count << " 0 D" <<endl;
}

//circuit containing variable number of diodes (without AC source)
void seriesDcDiode(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesDcDiode" <<endl;
    buffer << "V1 n1 0 5" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "D" << i << " n" << i << " n" << i+1 << " D" <<endl;
    }
    buffer << "D" << count << " n" << count << " 0 D" <<endl;
}

//N-channel mosfet 
void seriesNMos(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesNMos" <<endl;
    buffer << ".MODEL NMOS NMOS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "M" << i << " n" << i << " n" << i << "0 NMOS" <<endl;
    }
    buffer << "M" << count << " n" << count << " n" << count << " 0 NMOS" <<endl;
}


//P-channel mosfets
void seriesPMos(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesPMos" <<endl;
    buffer << ".MODEL PMOS PMOS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "M" << i << " n" << i << " n" << i << "0 PMOS" <<endl;
    }
    buffer << "M" << count << " n" << count << " n" << count << " 0 PMOS" <<endl;
}

//NPN BJT
void seriesNPN(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesNPN" <<endl;
    buffer << ".MODEL NPN NPN" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "Q" << i << " n" << i << " n" << i << "0 NPN" <<endl;
    }
    buffer << "Q" << count << " n" << count << " n" << count << " 0 NPN" <<endl;
}


//PNP BJT
void seriesPNP(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesPNP" <<endl;
    buffer << ".MODEL PNP PNP" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "Q" << i << " n" << i << " n" << i << "0 PNP" <<endl;
    }
    buffer << "Q" << count << " n" << count << " n" << count << " 0 PNP" <<endl;
}


int main(int argc, char **argv){
    //the following times will be default for the tests that don't involve changing of timestep and simulation time
    float timeStep = 10e-6; //seconds for linear (dynamic timestep for nonlinear)
    float simulationTime = 1; //seconds

    Circuit* c;
    stringstream buffer;
    ofstream outputFile;

    //timestep scaling test (simple circuit)
    c = new Circuit{};
    outputFile.open("output/timeStepTest.csv");
    outputFile << "Timestep (seconds), Simulation Time (seconds)" << endl;

    // how many timesteps to use
    float maxTimeStep1 = 1;
    float minTimeStep1 = 1e-6;
    float timeStepDivider1 = 1.1;
    // float deltaTimeStep1 = 0.0001;

    for(float timeStep{maxTimeStep1}; timeStep>=minTimeStep1; timeStep/=timeStepDivider1){
        auto start = high_resolution_clock::now(); 
        timestep1(buffer);

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");

        auto stop = high_resolution_clock::now(); 
        auto duration = duration_cast<microseconds>(stop - start);
        auto count = duration.count();
        outputFile << timeStep << "," << count/1000.0f <<endl;
    }

    delete c;
    outputFile.close();

    ///////////////////////////////////////////////////////////////////////////////////////////

    //timestep scaling test (more complicated circuit containing inductors/capacitors)
    c = new Circuit{};
    outputFile.open("output/timeStepTestComplex.csv");
    outputFile << "Timestep (seconds), Simulation Time (seconds)" << endl;
    
    // how many timesteps to use
    float maxTimeStep2 = 1;
    float minTimeStep2 = 1e-6;
    float timeStepDivider2 = 1.1;
    // float deltaTimeStep2 = 1e-7;

    float smallestTimestep2 = 1e-6;
    for(float timeStep{maxTimeStep2}; timeStep>=minTimeStep2; timeStep/=timeStepDivider2){
        auto start = high_resolution_clock::now(); 
        timestep2(buffer);

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");

        auto stop = high_resolution_clock::now(); 
        auto duration = duration_cast<microseconds>(stop - start);
        auto count = duration.count();
        outputFile << timeStep << "," << count/1e6f <<endl;
    }

    delete c;
    outputFile.close();

    ////////////////////////////////////////////////////////////////////////////////////////////

    //simulation time scaling test (simple circuit)
    c = new Circuit{};
    outputFile.open("output/timeScalingTest.csv");
    outputFile << "Simulation end time (seconds), Simulation Time (seconds)" << endl;
    
    // how many simulationTimes to use
    float minSimulationTime1 = 0.5;
    float maxSimulationTime1 = 30;
    float deltaSimulationTime1 = .5;
    
    for(float simulationTime{minSimulationTime1}; simulationTime<=maxSimulationTime1; simulationTime+=deltaSimulationTime1){
        auto start = high_resolution_clock::now(); 
        timestep1(buffer);

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");

        auto stop = high_resolution_clock::now(); 
        auto duration = duration_cast<microseconds>(stop - start);
        auto count = duration.count();
        outputFile << simulationTime << "," << count/1e6f <<endl;
    }

    delete c;
    outputFile.close();

    ////////////////////////////////////////////////////////////////////////////////////////////

    //simulation time scaling test (more complicated circuit containing inductors/capacitors)
    c = new Circuit{};
    outputFile.open("output/timeScalingTestComplex.csv");
    outputFile << "Simulation end time (seconds), Simulation Time (seconds)" << endl;
    
    // how many simulationTimes to use
    float minSimulationTime2 = 0.5;
    float maxSimulationTime2 = 30;
    float deltaSimulationTime2 = .5;

    for(float simulationTime{minSimulationTime2}; simulationTime<=maxSimulationTime2; simulationTime+=deltaSimulationTime2){
        auto start = high_resolution_clock::now(); 
        timestep2(buffer);

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");

        auto stop = high_resolution_clock::now(); 
        auto duration = duration_cast<microseconds>(stop - start);
        auto count = duration.count();
        outputFile << simulationTime << "," << count/1e6f <<endl;
    }

    delete c;
    outputFile.close();

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series resistor scaling test
    c = new Circuit{};

    outputFile.open("output/resistorsTest.csv");
    outputFile << "Resistor count, Simulation Time (seconds)" << endl;

    // how many resistors to use
    int minResistors = 0;
    int maxResistors = 50;
    int deltaResistors = 1;

    for(int resistorCount = minResistors; resistorCount < maxResistors; resistorCount += deltaResistors){
        seriesResistors(buffer, resistorCount);
        
        auto start = high_resolution_clock::now();

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << resistorCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    //////////////////////////////////////////////////////////////////////////////////////////////

    //series capacitor scaling test
    c = new Circuit{};

    outputFile.open("output/capacitorsTest.csv");
    outputFile << "Capacitor count, Simulation Time (seconds)" << endl;

    // how many capacitor to use
    int minCapacitors = 0;
    int maxCapacitors = 50;
    int deltaCapacitors = 1;

    for(int capacitorCount = minCapacitors; capacitorCount < maxCapacitors; capacitorCount += deltaCapacitors){
        seriesCapacitors(buffer, capacitorCount);
        
        auto start = high_resolution_clock::now();

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << capacitorCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    //////////////////////////////////////////////////////////////////////////////////////////////

    //series inductor scaling test
    c = new Circuit{};

    outputFile.open("output/inductorsTest.csv");
    outputFile << "Inductor count, Simulation Time (seconds)" << endl;

    // how many inductors to use
    int minInductors = 0;
    int maxInductors = 50;
    int deltaInductors = 1;

    for(int inductorCount = minInductors; inductorCount < maxInductors; inductorCount += deltaInductors){
        seriesInductors(buffer, inductorCount);
        
        auto start = high_resolution_clock::now();

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << inductorCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    //////////////////////////////////////////////////////////////////////////////////////////////

    //series vs scaling test
    c = new Circuit{};

    outputFile.open("output/VSTest.csv");
    outputFile << "VS count, Simulation Time (seconds)" << endl;

    // how many VS to use
    int minVS = 0;
    int maxVS = 50;
    int deltaVS = 1;

    for(int VSCount = minVS; VSCount < maxVS; VSCount += deltaVS){
        seriesVS(buffer, VSCount);
        
        auto start = high_resolution_clock::now();

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << VSCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series cs scaling test
    c = new Circuit{};

    outputFile.open("output/CSTest.csv");
    outputFile << "CS count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minCS = 0;
    int maxCS = 50;
    int deltaCS = 1;

    for(int CSCount = minCS; CSCount < maxCS; CSCount += deltaCS){
        seriesCS(buffer, CSCount);
        
        auto start = high_resolution_clock::now();

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << CSCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////


    //series op-amp scaling test
    c = new Circuit{};

    outputFile.open("output/OpAmpTest.csv");
    outputFile << "OpAmp count, Simulation Time (seconds)" << endl;

    // how many op-amps to use
    int minOpAmp = 0;
    int maxOpAmp = 50;
    int deltaOpAmp = 1;

    for(int OpAmpCount = minOpAmp; OpAmpCount < maxOpAmp; OpAmpCount += deltaOpAmp){
        seriesOpAmps(buffer, OpAmpCount);
        
        auto start = high_resolution_clock::now();

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << OpAmpCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series VCVS scaling test
    c = new Circuit{};

    outputFile.open("output/VCVSTest.csv");
    outputFile << "VCVS count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minVCVS = 0;
    int maxVCVS = 50;
    int deltaVCVS = 1;

    for(int VCVSCount = minVCVS; VCVSCount < maxVCVS; VCVSCount += deltaVCVS){
        seriesVCVS(buffer, VCVSCount);
        
        auto start = high_resolution_clock::now();

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << VCVSCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series VCCS scaling test
    c = new Circuit{};

    outputFile.open("output/VCCSTest.csv");
    outputFile << "VCCS count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minVCCS = 0;
    int maxVCCS = 50;
    int deltaVCCS = 1;

    for(int VCCSCount = minVCCS; VCCSCount < maxVCCS; VCCSCount += deltaVCCS){
        seriesVCCS(buffer, VCCSCount);
        
        auto start = high_resolution_clock::now();

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << VCCSCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series CCVS scaling test
    c = new Circuit{};

    outputFile.open("output/CCVSTest.csv");
    outputFile << "CCVS count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minCCVS = 0;
    int maxCCVS = 50;
    int deltaCCVS = 1;

    for(int CCVSCount = minCCVS; CCVSCount < maxCCVS; CCVSCount += deltaCCVS){
        seriesCCVS(buffer, CCVSCount);
        
        auto start = high_resolution_clock::now();

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << CCVSCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series CCCS scaling test
    c = new Circuit{};

    outputFile.open("output/CCCSTest.csv");
    outputFile << "CCCS count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minCCCS = 0;
    int maxCCCS = 50;
    int deltaCCCS = 1;

    for(int CCCSCount = minCCCS; CCCSCount < maxCCCS; CCCSCount += deltaCCCS){
        seriesCCCS(buffer, CCCSCount);
        
        auto start = high_resolution_clock::now();

        c.setTimeStep(timeStep);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << CCCSCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series ac diode scaling test
    c = new Circuit{};

    outputFile.open("output/AcDiodeTest.csv");
    outputFile << "AcDiode count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minAcDiode = 0;
    int maxAcDiode = 50;
    int deltaAcDiode = 1;

    for(int AcDiodeCount = minAcDiode; AcDiodeCount < maxAcDiode; AcDiodeCount += deltaAcDiode){
        seriesAcDiode(buffer, AcDiodeCount);
        
        auto start = high_resolution_clock::now();

        c.setHasNonLinearComponents(true);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << AcDiodeCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series dc diode scaling test
    c = new Circuit{};

    outputFile.open("output/DcDiodeTest.csv");
    outputFile << "DcDiode count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minDcDiode = 0;
    int maxDcDiode = 50;
    int deltaDcDiode = 1;

    for(int DcDiodeCount = minDcDiode; DcDiodeCount < maxDcDiode; DcDiodeCount += deltaDcDiode){
        seriesDcDiode(buffer, DcDiodeCount);
        
        auto start = high_resolution_clock::now();

        c.setHasNonLinearComponents(true);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << DcDiodeCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series NMos scaling test
    c = new Circuit{};

    outputFile.open("output/NMosTest.csv");
    outputFile << "NMos count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minNMos = 0;
    int maxNMos = 50;
    int deltaNMos = 1;

    for(int NMosCount = minNMos; NMosCount < maxNMos; NMosCount += deltaNMos){
        seriesNMos(buffer, NMosCount);
        
        auto start = high_resolution_clock::now();

        c.setHasNonLinearComponents(true);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << NMosCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series PMos scaling test
    c = new Circuit{};

    outputFile.open("output/PMosTest.csv");
    outputFile << "PMos count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minPMos = 0;
    int maxPMos = 50;
    int deltaPMos = 1;

    for(int PMosCount = minPMos; PMosCount < maxPMos; PMosCount += deltaPMos){
        seriesPMos(buffer, PMosCount);
        
        auto start = high_resolution_clock::now();

        c.setHasNonLinearComponents(true);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << PMosCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series NPN scaling test
    c = new Circuit{};

    outputFile.open("output/NPNTest.csv");
    outputFile << "NPN count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minNPN = 0;
    int maxNPN = 50;
    int deltaNPN = 1;

    for(int NPNCount = minNPN; NPNCount < maxNPN; NPNCount += deltaNPN){
        seriesNPN(buffer, NPNCount);
        
        auto start = high_resolution_clock::now();

        c.setHasNonLinearComponents(true);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << NPNCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series PNP scaling test
    c = new Circuit{};

    outputFile.open("output/PNPTest.csv");
    outputFile << "PNP count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minPNP = 0;
    int maxPNP = 50;
    int deltaPNP = 1;

    for(int PNPCount = minPNP; PNPCount < maxPNP; PNPCount += deltaPNP){
        seriesPNP(buffer, PNPCount);
        
        auto start = high_resolution_clock::now();

        c.setHasNonLinearComponents(true);
        c.setTStep(timeStep);
        c.setSimulationTime(simulationTime);
        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << PNPCount << "," << timeTaken/1e6f << endl;
    }

    outputFile.close();
    delete c;

    ////////////////////////////////////////////////////////////////////////////////////////////
}