#include <iostream>
#include <vector>
#include <fstream>
#include <chrono> 

#include <circuit/circuit.hpp>
#include <input/input.hpp>
#include <output/output.hpp>

// for debugging only
// #include <circuit/circuit.cpp>

// #include <component/bjt.cpp>
// #include <component/capacitor.cpp>
// #include <component/component.cpp>
// #include <component/currentControlledCurrentSource.cpp>
// #include <component/currentControlledSource.cpp>
// #include <component/currentControlledVoltageSource.cpp>
// #include <component/currentSource.cpp>
// #include <component/diode.cpp>
// #include <component/exampleInputDiode.hpp>
// #include <component/inductor.cpp>
// #include <component/mosfet.cpp>
// #include <component/opamp.cpp>
// #include <component/resistor.cpp>
// #include <component/voltageControlledCurrentSource.cpp>
// #include <component/voltageControlledSource.cpp>
// #include <component/voltageControlledVoltageSource.cpp>
// #include <component/voltageSource.cpp>
// #include <component/waveform.cpp>

// #include <input/input.cpp>

// #include <output/linearAnalysis.cpp>
// #include <output/nonLinearAnalysis.cpp>
// #include <output/output.cpp>

using namespace std;
using namespace std::chrono; 

int maxCompPerSim = 51;
float maxSimTime = 1.8E+09;

//simple test circuit that is used for dynamic timestep/ dynamic simulation time test
void timestep1(stringstream& buffer){
    buffer.clear();
    buffer << "timestep1" << endl;
    buffer << "V1 n1 0 SIN(0 10 10)" << endl;
    buffer << "R1 n2 0 1k" << endl;
    buffer << "R2 n1 n2 500" << endl;
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}
//more complex test circuit that is used for dynamic timestep/ dynamic simulation time test
void timestep2(stringstream& buffer){
    buffer.clear();
    buffer << "timestep2" << endl;
    buffer << "V1 n2 n1 SIN(0 10 10)" << endl;
    buffer << "R1 n3 n2 2k" << endl;
    buffer << "L1 n1 0 .1" << endl;
    buffer << "R2 n3 0 5k" << endl;
    buffer << "I1 n4 n3 .5" << endl;
    buffer << "R3 n4 0 10k" << endl;
    buffer << "C1 n4 0 1u" << endl;
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}


//circuit containing variable number of series resistors
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

//circuit containing variable number of series capacitors
void seriesCapacitors(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesCapacitors" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        buffer << "C" << i << " n" << i << " n" << i+1 << " 1u" <<endl;
    }
    buffer << "C" << count << " n" << count << " 0 1u" <<endl;
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
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
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
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
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
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
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

//circuit containing variable number of op-amps
//negative terminal connected with output + positive terminal connected to ground
void seriesOpAmps(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesOpAmps" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<=count; i++){
        buffer << "X" << i << " N" << i << " 0" << " N" << i << " opamp" <<endl;
    }
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

//voltage controlled voltage source
void seriesVCVS(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesVCVS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        if(i == 1){
            buffer << "E" << i << " n" << i << " n" << i+1 << " " << 0 << " n" << i << " 2" <<endl;
        }else{
            buffer << "E" << i << " n" << i << " n" << i+1 << " n" << i-1 << " n" << i << " 2" <<endl;
        }
    }
    if(count == 1){
        buffer << "E" << count << " n" << count << " 0 " << 0 << " n" << count << " 2" <<endl;
    }else{
        buffer << "E" << count << " n" << count << " 0 n" << count-1 << " n" << count << " 2" <<endl;
    }
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

//voltage controlled current source
void seriesVCCS(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesVCCS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i<count; i++){
        if(i == 1){
            buffer << "G" << i << " n" << i << " n" << i+1 << " " << 0 << " n" << i << " 2" <<endl;
        }else{
            buffer << "G" << i << " n" << i << " n" << i+1 << " n" << i-1 << " n" << i << " 2" <<endl;
        }
    }
    if(count == 1){
        buffer << "G" << count << " n" << count << " 0 " << 0 << " n" << count << " 2" <<endl;
    }else{
        buffer << "G" << count << " n" << count << " 0 n" << count-1 << " n" << count << " 2" <<endl;
    }
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
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
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
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
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

//circuit containing variable number of diodes (with AC source)
void seriesAcDiode(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesAcDiode" << endl;
    buffer << ".model D D" << endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    for(u_int i{1}; i < count+1; i++){
        buffer << "D" << i << " n" << i << " n" << i+1 << " D" <<endl;
    }
    buffer << "R" << count << " n" << count << " 0 1k" <<endl;
    buffer << ".tran 0.001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

//circuit containing variable number of diodes (without AC source)
void seriesDcDiode(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesDcDiode" <<endl;
    buffer << ".model D D" << endl;
    buffer << "V1 n1 0 5" <<endl;
    for(u_int i{1}; i<count+1; i++){
        buffer << "D" << i << " n" << i << " n" << i+1 << " D" <<endl;
    }
    buffer << "R" << count << " n" << count << " 0 1k" <<endl;
    buffer << ".tran 0.001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

//N-channel mosfet 
void seriesNMos(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesNMos" <<endl;
    buffer << ".MODEL NMOS NMOS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    buffer << "R2 n1 n2 10k" << endl;
    for(u_int i{1}; i<count + 1; i++){
        buffer << "M" << i << " 0 n" << i+1 << " n" << i+2 << " 0 NMOS" << endl;
    }
    buffer << "R1" << " n" << count+2 << " 0 10k" <<endl;
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

//P-channel mosfets
void seriesPMos(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesPMos" <<endl;
    buffer << ".MODEL PMOS PMOS" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    buffer << "R2 n1 n2 10k" << endl;
    for(u_int i{1}; i<count + 1; i++){
        buffer << "M" << i << " 0 n" << i+1 << " n" << i+2 << " 0 PMOS" << endl;
    }
    buffer << "R1" << " n" << count+2 << " 0 10k" <<endl;
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

//NPN BJT
void seriesNPN(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesNPN" <<endl;
    buffer << ".MODEL NPN NPN" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    buffer << "R2 n1 n2 10k" << endl;
    for(u_int i{1}; i<count+1; i++){
        buffer << "Q" << i << " 0 n" << i+1 << " n" << i+2 << " 0 NPN" << endl;
    }
    buffer << "R1" << " n" << count+2 << " 0 10k" <<endl;
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}

//PNP BJT
void seriesPNP(stringstream& buffer, u_int count){
    buffer.clear();
    buffer << "seriesPNP" <<endl;
    buffer << ".MODEL PNP PNP" <<endl;
    buffer << "V1 n1 0 SIN(0 10 10)" <<endl;
    buffer << "R2 n1 n2 10k" << endl;
    for(u_int i{1}; i<count+1; i++){
        buffer << "Q" << i << " 0 n" << i+1 << " n" << i+2 << " 0 PNP" << endl;
    }
    buffer << "R1" << " n" << count+2 << " 0 10k" <<endl;
    buffer << ".tran 0.0001 0.5 0" <<endl;
    buffer << ".end" <<endl;
}


int main(int argc, char **argv){
    //the following times will be default for the tests that don't involve changing of timestep and simulation time
    float timeStep = 10e-6; //seconds for linear (dynamic timestep for nonlinear)
    float simulationTime = 1; //seconds

    Circuit* c;
    stringstream buffer;
    ofstream outputFile;
{}
{
    //////////////////////////////////////////////////////////////////////////////////////////
    // // timestep scaling test (simple circuit)
    // c = new Circuit{};
    // outputFile.open("output/timeStepTest.csv");
    // outputFile << "Timestep (seconds), Simulation Time (seconds)" << endl;

    // // how many timesteps to use
    // float maxTimeStep1 = 1;
    // float minTimeStep1 = 1e-6;
    // float timeStepDivider1 = 1.1;
    // // float deltaTimeStep1 = 0.0001;

    // for(float timeStep{maxTimeStep1}; timeStep>=minTimeStep1; timeStep/=timeStepDivider1){
    //     auto start = high_resolution_clock::now(); 
    //     timestep1(buffer);

    //     c->setTimeStep(timeStep);
    //     c->setTStep(timeStep);
    //     readSpice(*c, buffer);
    //     outputCSV(*c, "output/ignore.csv");

    //     auto stop = high_resolution_clock::now(); 
    //     auto duration = duration_cast<microseconds>(stop - start);
    //     auto count = duration.count();
    //     outputFile << timeStep << "," << count/1000.0f <<endl;
    // }

    // delete c;
    // outputFile.close();

    ///////////////////////////////////////////////////////////////////////////////////////////

    // //timestep scaling test (more complicated circuit containing inductors/capacitors)
    // c = new Circuit{};
    // outputFile.open("output/timeStepTestComplex.csv");
    // outputFile << "Timestep (seconds), Simulation Time (seconds)" << endl;
    
    // // how many timesteps to use
    // float maxTimeStep2 = 1;
    // float minTimeStep2 = 1e-6;
    // float timeStepDivider2 = 1.1;
    // // float deltaTimeStep2 = 1e-7;

    // float smallestTimestep2 = 1e-6;
    // for(float timeStep{maxTimeStep2}; timeStep>=minTimeStep2; timeStep/=timeStepDivider2){
    //     auto start = high_resolution_clock::now(); 
    //     timestep2(buffer);

    //     c->setTimeStep(timeStep);
    //     c->setTStep(timeStep);
    //     readSpice(*c, buffer);
    //     outputCSV(*c, "output/ignore.csv");

    //     auto stop = high_resolution_clock::now(); 
    //     auto duration = duration_cast<microseconds>(stop - start);
    //     auto count = duration.count();
    //     outputFile << timeStep << "," << count/1e6f <<endl;
    // }

    // delete c;
    // outputFile.close();

    ////////////////////////////////////////////////////////////////////////////////////////////

    // //simulation time scaling test (simple circuit)
    // c = new Circuit{};
    // outputFile.open("output/timeScalingTest.csv");
    // outputFile << "Simulation end time (seconds), Simulation Time (seconds)" << endl;
    
    // // how many simulationTimes to use
    // float minSimulationTime1 = 0.5;
    // float maxSimulationTime1 = 30;
    // float deltaSimulationTime1 = .5;
    
    // for(float simulationTime{minSimulationTime1}; simulationTime<=maxSimulationTime1; simulationTime+=deltaSimulationTime1){
    //     auto start = high_resolution_clock::now(); 
    //     timestep1(buffer);

    //     c->setSimulationTime(simulationTime);
    //     readSpice(*c, buffer);
    //     outputCSV(*c, "output/ignore.csv");

    //     auto stop = high_resolution_clock::now(); 
    //     auto duration = duration_cast<microseconds>(stop - start);
    //     auto count = duration.count();
    //     outputFile << simulationTime << "," << count/1e6f <<endl;
    // }

    // delete c;
    // outputFile.close();

    ////////////////////////////////////////////////////////////////////////////////////////////

    // //simulation time scaling test (more complicated circuit containing inductors/capacitors)
    // c = new Circuit{};
    // outputFile.open("output/timeScalingTestComplex.csv");
    // outputFile << "Simulation end time (seconds), Simulation Time (seconds)" << endl;
    
    // // how many simulationTimes to use
    // float minSimulationTime2 = 0.5;
    // float maxSimulationTime2 = 30;
    // float deltaSimulationTime2 = .5;

    // for(float simulationTime{minSimulationTime2}; simulationTime<=maxSimulationTime2; simulationTime+=deltaSimulationTime2){
    //     auto start = high_resolution_clock::now(); 
    //     timestep2(buffer);

    //     c->setSimulationTime(simulationTime);
    //     readSpice(*c, buffer);
    //     outputCSV(*c, "output/ignore.csv");

    //     auto stop = high_resolution_clock::now(); 
    //     auto duration = duration_cast<microseconds>(stop - start);
    //     auto count = duration.count();
    //     outputFile << simulationTime << "," << count/1e6f <<endl;
    // }

    // delete c;
    // outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series resistor scaling test
{}
{
    outputFile.open("output/resistorsTest.csv");
    outputFile << "Resistor count, Simulation Time (seconds)" << endl;

    // how many resistors to use
    int minResistors = 1;
    int maxResistors = maxCompPerSim;
    int deltaResistors = 1;

    for(int resistorCount = minResistors; resistorCount < maxResistors; resistorCount += deltaResistors){
        c = new Circuit{};
        
        seriesResistors(buffer, resistorCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << resistorCount << "," << timeTaken/1e6f << endl;
        if(timeTaken > maxSimTime){
            cerr << "Resistors maxed out time";
            resistorCount = maxResistors;
        }

        delete c;
    }

    outputFile.close();
}

    //////////////////////////////////////////////////////////////////////////////////////////////

    //series capacitor scaling test
{
    outputFile.open("output/capacitorsTest.csv");
    outputFile << "Capacitor count, Simulation Time (seconds)" << endl;

    // how many capacitor to use
    int minCapacitors = 1;
    int maxCapacitors = maxCompPerSim;
    int deltaCapacitors = 1;

    for(int capacitorCount = minCapacitors; capacitorCount < maxCapacitors; capacitorCount += deltaCapacitors){
        c = new Circuit{};

        seriesCapacitors(buffer, capacitorCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << capacitorCount << "," << timeTaken/1e6f << endl;
        if(timeTaken > maxSimTime){
            cerr << "Capacitors maxed out time";
            capacitorCount = maxCapacitors;
        }

        delete c;
    }

    outputFile.close();
}

    //////////////////////////////////////////////////////////////////////////////////////////////

    //series inductor scaling test
{
    outputFile.open("output/inductorsTest.csv");
    outputFile << "Inductor count, Simulation Time (seconds)" << endl;

    // how many inductors to use
    int minInductors = 1;
    int maxInductors = maxCompPerSim;
    int deltaInductors = 1;

    for(int inductorCount = minInductors; inductorCount < maxInductors; inductorCount += deltaInductors){
        c = new Circuit{};
        
        seriesInductors(buffer, inductorCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << inductorCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "Inductors maxed out time";
            inductorCount = maxInductors;
        }

        delete c;
    }

    outputFile.close();
}

    //////////////////////////////////////////////////////////////////////////////////////////////

    //series vs scaling test
{
    outputFile.open("output/VSTest.csv");
    outputFile << "VS count, Simulation Time (seconds)" << endl;

    // how many VS to use
    int minVS = 1;
    int maxVS = maxCompPerSim;
    int deltaVS = 1;

    for(int VSCount = minVS; VSCount < maxVS; VSCount += deltaVS){
        c = new Circuit{};
        
        seriesVS(buffer, VSCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << VSCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "VS maxed out time";
            VSCount = maxVS;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series cs scaling test
{
    outputFile.open("output/CSTest.csv");
    outputFile << "CS count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minCS = 1;
    int maxCS = maxCompPerSim;
    int deltaCS = 1;

    for(int CSCount = minCS; CSCount < maxCS; CSCount += deltaCS){
        c = new Circuit{};
        
        seriesCS(buffer, CSCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << CSCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "CS maxed out time";
            CSCount = maxCS;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series op-amp scaling test
{
    outputFile.open("output/OpAmpTest.csv");
    outputFile << "OpAmp count, Simulation Time (seconds)" << endl;

    // how many op-amps to use
    int minOpAmp = 1;
    int maxOpAmp = maxCompPerSim;
    int deltaOpAmp = 1;

    for(int OpAmpCount = minOpAmp; OpAmpCount < maxOpAmp; OpAmpCount += deltaOpAmp){
        c = new Circuit{};
        
        seriesOpAmps(buffer, OpAmpCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << OpAmpCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "OpAmp maxed out time";
            OpAmpCount = maxOpAmp;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series VCVS scaling test
{
    outputFile.open("output/VCVSTest.csv");
    outputFile << "VCVS count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minVCVS = 1;
    int maxVCVS = maxCompPerSim;
    int deltaVCVS = 1;

    for(int VCVSCount = minVCVS; VCVSCount < maxVCVS; VCVSCount += deltaVCVS){
        c = new Circuit{};
        
        seriesVCVS(buffer, VCVSCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << VCVSCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "VCVS maxed out time";
            VCVSCount = maxVCVS;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series VCCS scaling test
{
    outputFile.open("output/VCCSTest.csv");
    outputFile << "VCCS count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minVCCS = 1;
    int maxVCCS = maxCompPerSim;
    int deltaVCCS = 1;

    for(int VCCSCount = minVCCS; VCCSCount < maxVCCS; VCCSCount += deltaVCCS){
        c = new Circuit{};
        
        seriesVCCS(buffer, VCCSCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << VCCSCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "VCCS maxed out time";
            VCCSCount = maxVCCS;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series CCVS scaling test
{
    outputFile.open("output/CCVSTest.csv");
    outputFile << "CCVS count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minCCVS = 1;
    int maxCCVS = maxCompPerSim;
    int deltaCCVS = 1;

    for(int CCVSCount = minCCVS; CCVSCount < maxCCVS; CCVSCount += deltaCCVS){
        c = new Circuit{};
        
        seriesCCVS(buffer, CCVSCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << CCVSCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "CCVS maxed out time";
            CCVSCount = maxCCVS;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series CCCS scaling test
{
    outputFile.open("output/CCCSTest.csv");
    outputFile << "CCCS count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minCCCS = 1;
    int maxCCCS = maxCompPerSim;
    int deltaCCCS = 1;

    for(int CCCSCount = minCCCS; CCCSCount < maxCCCS; CCCSCount += deltaCCCS){
        c = new Circuit{};
        
        seriesCCCS(buffer, CCCSCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << CCCSCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "CCCS maxed out time";
            CCCSCount = maxCCCS;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series ac diode scaling test
{
    outputFile.open("output/AcDiodeTest.csv");
    outputFile << "AcDiode count, Simulation Time (seconds)" << endl;

    // how many diodes to use
    int minAcDiode = 1;
    int maxAcDiode = maxCompPerSim;
    int deltaAcDiode = 1;

    for(int AcDiodeCount = minAcDiode; AcDiodeCount < maxAcDiode; AcDiodeCount += deltaAcDiode){
        c = new Circuit{};

        seriesAcDiode(buffer, AcDiodeCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();

        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << AcDiodeCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "AcDiode maxed out time";
            AcDiodeCount = maxAcDiode;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series dc diode scaling test
{
    outputFile.open("output/DcDiodeTest.csv");
    outputFile << "DcDiode count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minDcDiode = 1;
    int maxDcDiode = maxCompPerSim;
    int deltaDcDiode = 1;

    for(int DcDiodeCount = minDcDiode; DcDiodeCount < maxDcDiode; DcDiodeCount += deltaDcDiode){
        c = new Circuit{};
        
        seriesDcDiode(buffer, DcDiodeCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << DcDiodeCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "DcDiode maxed out time";
            DcDiodeCount = maxDcDiode;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    // //series NMos scaling test
{
    // outputFile.open("output/NMosTest.csv");
    // outputFile << "NMos count, Simulation Time (seconds)" << endl;

    // // how many CS to use
    // int minNMos = 1;
    // int maxNMos = maxCompPerSim;
    // int deltaNMos = 1;

    // for(int NMosCount = minNMos; NMosCount < maxNMos; NMosCount += deltaNMos){
    //     c = new Circuit{};
        
    //     seriesNMos(buffer, NMosCount);
        
    //     auto start = high_resolution_clock::now();

    //     readSpice(*c, buffer);
    //     outputCSV(*c, "output/ignore.csv");
        
    //     auto stop = high_resolution_clock::now();
    //     auto duration = duration_cast<microseconds>(stop - start);
    //     auto timeTaken = duration.count();
    //     outputFile << NMosCount << "," << timeTaken/1e6f << endl;
        
    //     if(timeTaken > maxSimTime){
    //         cerr << "NMOS maxed out time";
    //         NMosCount = maxNMos;
    //     }

    //     delete c;
    // }

    // outputFile.close();
}

    // ////////////////////////////////////////////////////////////////////////////////////////////

    // series PMos scaling test
{
//     outputFile.open("output/PMosTest.csv");
//     outputFile << "PMos count, Simulation Time (seconds)" << endl;

//     // how many CS to use
//     int minPMos = 1;
//     int maxPMos = maxCompPerSim;
//     int deltaPMos = 1;

//     for(int PMosCount = minPMos; PMosCount < maxPMos; PMosCount += deltaPMos){
//         c = new Circuit{};
        
//         seriesPMos(buffer, PMosCount);
        
//         auto start = high_resolution_clock::now();

//         readSpice(*c, buffer);
//         outputCSV(*c, "output/ignore.csv");
        
//         auto stop = high_resolution_clock::now();
//         auto duration = duration_cast<microseconds>(stop - start);
//         auto timeTaken = duration.count();
//         outputFile << PMosCount << "," << timeTaken/1e6f << endl;
        
//         if(timeTaken > maxSimTime){
//             cerr << "PMOS maxed out time";
//             PMosCount = maxPMos;
//         }

//         delete c;
//     }

//     outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series NPN scaling test
{
    outputFile.open("output/NPNTest.csv");
    outputFile << "NPN count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minNPN = 1;
    int maxNPN = maxCompPerSim;
    int deltaNPN = 1;

    for(int NPNCount = minNPN; NPNCount < maxNPN; NPNCount += deltaNPN){
        c = new Circuit{};
        
        seriesNPN(buffer, NPNCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << NPNCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "NPN maxed out time";
            NPNCount = maxNPN;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////

    //series PNP scaling test
{
    outputFile.open("output/PNPTest.csv");
    outputFile << "PNP count, Simulation Time (seconds)" << endl;

    // how many CS to use
    int minPNP = 1;
    int maxPNP = maxCompPerSim;
    int deltaPNP = 1;

    for(int PNPCount = minPNP; PNPCount < maxPNP; PNPCount += deltaPNP){
        c = new Circuit{};
        
        seriesPNP(buffer, PNPCount);
        
        auto start = high_resolution_clock::now();

        readSpice(*c, buffer);
        outputCSV(*c, "output/ignore.csv");
        
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        auto timeTaken = duration.count();
        outputFile << PNPCount << "," << timeTaken/1e6f << endl;
        
        if(timeTaken > maxSimTime){
            cerr << "PNP maxed out time";
            PNPCount = maxPNP;
        }

        delete c;
    }

    outputFile.close();
}

    ////////////////////////////////////////////////////////////////////////////////////////////
}