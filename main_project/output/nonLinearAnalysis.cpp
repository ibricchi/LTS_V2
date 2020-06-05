#include <vector>
#include <string>
#include <circuit/circuit.hpp>
#include <Eigen/Dense>
#include <component/component.hpp>
#include <component/voltageSource.hpp>

#include "nonLinearAnalysis.hpp"

void nonLinearSetup(Circuit& c){
    c.setupCurrentControlledSources(c); //add idx of the controlling voltage source (must come prior to setting up A)
    c.nlSetup();
};

string runNonLinearTransience(Circuit& c, float t){
    //get references to the components stored inside the circuit
    vector<Component*> components = c.getComponentsRef();
    vector<Component*> voltageSources = c.getVoltageSourcesRef();
    vector<Component*> currentSources = c.getCurrentSourcesRef();
    vector<Component*> conductanceSources = c.getConductanceSourcesRef();
    vector<Component*> vcUpdatables = c.getVCUpdatablesRef();
    vector<Component*> timeUpdatables = c.getTimeUpdatablesRef();
    int highestNodeNumber = c.getHighestNodeNumber();
if(t==11){cerr << "Final One!" << endl;}
    //forms a row in the csv file
    string outLine{};
cerr << "Time: " << t << endl;
    float threshold = .001;

    //c.updateNodalVoltages();
    VectorXd startX = c.getX();
    VectorXd currentX = c.getX();
    VectorXd newX = c.getX();

    // IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    // cout << newX.format(CleanFmt) << endl << endl;

    // keep calculating for current time step till threshold is bellow ceratin level
    int count = 0;
    int maxCount = 500;
    int dynamicTimeStepMaxCount = 3;
    int dynamicTimeStepMinCount = 2;
    int dynamicTimeStepFactor = 8;
    float dynamicTimeStepAbsoluteDelta = 0.1;
    float prevTime = c.getPrevTime();    
double dynamicTimeStep = (prevTime==0 ? c.getTimeStep() : t-c.getPrevTime());
    cerr << "DynTimeStep: " << dynamicTimeStep << endl;
    if(dynamicTimeStep ==0){exit(1);}
    float gamma = 0.1;
    double minDynamicTimeStep = c.getSimulationTime()/50000000000;
    double maxDynamicTimeStep = c.getSimulationTime()/50;

    for(const auto &up : vcUpdatables){
	up->setTimeStep(dynamicTimeStep);
    }

    for(const auto &timeUp : timeUpdatables){
	timeUp->updateVals(t);
    }
    do{
        if(count > maxCount){
            cerr << "Newton Raphson count too big" <<endl;
            exit(1);
        }
	if((count > dynamicTimeStepMaxCount || !matrixDiffBellowThreshold(startX,newX,dynamicTimeStepAbsoluteDelta)) && (dynamicTimeStep>=dynamicTimeStepFactor*minDynamicTimeStep)){
	    cerr << "NextTimeStep: " << dynamicTimeStep/dynamicTimeStepFactor << endl;
	    cerr << "StartX" << endl << startX << endl << "StartX" << endl;
	    c.setX(startX);
	    c.setTimeStep(dynamicTimeStep/dynamicTimeStepFactor);
	    outLine = runNonLinearTransience(c,c.getPrevTime()+dynamicTimeStep/dynamicTimeStepFactor);
	    return outLine;   
	}
	cerr << "Newton-Raphson Count: " << count << endl;        
	c.nonLinearA();
        c.computeA_inv();
        c.nonLinearB();
        c.computeNLX(gamma); //simply does A_inv*b (same as for linear x)
        currentX = newX;
        newX = c.getX();
        c.updateNodalVoltages(); //update based on newX
	cerr << newX << endl;
	cerr << currentX << endl;
        // IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
        // cout << endl << "t " << t << ":" << endl << "-------------------------------" << endl;
        // cout << "A: " << endl << c.getA().format(CleanFmt) << endl << endl;
        // cout << c.getA_inv().format(CleanFmt) << endl << endl;
        // cout << "B: " << endl << c.getB().format(CleanFmt) << endl << endl;
        // cout << "Old x: " << endl << currentX.format(CleanFmt) << endl << endl;
        // cout << "New x: " << endl << newX.format(CleanFmt) << endl << endl;

        count++;
    }
    while(!matrixDiffBellowThreshold(currentX, newX, threshold));
	cerr << "Made it past while" << endl;    
if(matrixDiffBellowThreshold(startX,newX,0.1)){    
	if(count < dynamicTimeStepMinCount && dynamicTimeStep <= maxDynamicTimeStep/2 ){
	dynamicTimeStep *= 2;
    }
}
    //output current time 
    c.setTimeStep(dynamicTimeStep); 
    c.setCurrentTime(t); //Do we need this?
    outLine += to_string(t);
cerr << "Past while dynamTime: " << dynamicTimeStep << endl;
    //output node voltages
    for(int i{}; i<highestNodeNumber; i++){
        outLine += "," + to_string(newX(i));
    }

    //output component currents
    for(const auto &comp : components){
        outLine += "," + to_string(comp->getTotalCurrent(newX, highestNodeNumber));
    }

   /* // update components before next calculation of b, commented out now as it is done at the beginning of function call with current value of t
    for(const auto &comp : timeUpdatables){
        comp->updateVals(t+c.getTimeStep());
    }
*/
    //update components based on current voltage/current
    float currentVoltage{}, currentCurrent{};
    for(const auto &up : vcUpdatables){
        auto nodes = up->getNodes();

        float v1 = nodes.at(0) == 0 ? 0 : newX(nodes.at(0)-1);
        float v2 = nodes.at(1) == 0 ? 0 : newX(nodes.at(1)-1);
        currentVoltage = v1 - v2;

        up->updateVals(currentVoltage, 0, 1);
    }
    c.setPrevTime(t);
    cerr << "Just before return dynTimeStep: " << dynamicTimeStep << endl;
    return outLine;
    
};

// both matrixes are assumed to be x:1 matrixes with same x
bool matrixDiffBellowThreshold(VectorXd& m1, VectorXd& m2, float d){
    for(int i = 0; i < m1.rows(); i++){
        // cout << m1(i) << " " << m2(i) << endl << endl;
        if(abs(m1(i) - m2(i)) > d){
            return false;
        }
    }
    return true;
}
