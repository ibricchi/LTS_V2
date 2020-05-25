#include <vector>
#include <string>
#include <circuit/circuit.hpp>
#include <Eigen/Dense>
#include <component/component.hpp>

#include "nonLinearAnalysis.hpp"

void nonLinearSetup(Circuit& c){
    c.nlSetup();
};

string runNonLinearTransience(Circuit& c, float t){
    //get references to the components stored inside the circuit
    vector<Component*> voltageSources = c.getVoltageSourcesRef();
    vector<Component*> currentSources = c.getCurrentSourcesRef();
    vector<Component*> conductanceSources = c.getConductanceSourcesRef();
    vector<Component*> vcUpdatables = c.getVCUpdatablesRef();
    vector<Component*> timeUpdatables = c.getTimeUpdatablesRef();
    int highestNodeNumber = c.getHighestNodeNumber();

    string outLine{};

    float threshold = .001;

    // c.updateNodalVoltages();
    VectorXd startX = c.getX();
    VectorXd currentX = c.getX();
    VectorXd newX = c.getX();

    // IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    // cout << newX.format(CleanFmt) << endl << endl;

    // keep calculating for current time step till threshold is bellow ceratin level
    int count = 0;
    int maxCount = 5;
    float gamma = 0.1;

    do{
        if(count > 500){
            cerr << "Newton Raphson count too big" <<endl;
            exit(1);
        }

        // if(count > maxCount){
        //     c.setX(startX);
        //     c.updateNodalVoltages();
            
        //     VectorXd currentX = c.getX();
        //     VectorXd newX = c.getX();

        //     gamma *= 0.9;
        //     maxCount += 1;
        // }
        // cout << newX.format(CleanFmt) << endl << endl;
        c.nonLinearA();
        c.computeA_inv();
        c.nonLinearB();
        c.computeNLX(gamma);
        c.updateNodalVoltages();
        currentX = newX;
        newX = c.getX();

        // IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
        // cout << endl << "t " << t << ":" << endl << "-------------------------------" << endl;
        // cout << "A: " << endl << c.getA().format(CleanFmt) << endl << endl;
        // cout << c.getA_inv().format(CleanFmt) << endl << endl;
        // cout << "B: " << endl << c.getB().format(CleanFmt) << endl << endl;
        // cout << "Old x: " << endl << currentX.format(CleanFmt) << endl << endl;
        // cout << "New x: " << endl << newX.format(CleanFmt) << endl << endl;

        // if(t >= 0.009780){
        //     count++;
        // }

        count++;
    }
    while(!matrixDiffBellowThreshold(currentX, newX, threshold));

    //output current time 
    c.setCurrentTime(t);
    outLine += to_string(t);

    //output node voltages
    for(int i{}; i<highestNodeNumber; i++){
        outLine += "," + to_string(newX(i));
    }

    // update components before next calculation of b
    for(const auto &comp : timeUpdatables){
        comp->updateVals(t+c.getTimeStep());
    }

    vector<int> nodes;
    
    //update components based on current voltage/current
    float v1{}, v2{}, currentVoltage{}, currentCurrent{};
    for(const auto &up : vcUpdatables){
        nodes = up->getNodes();

        v1 = nodes.at(0) == 0 ? 0 : newX(nodes.at(0)-1);
        v2 = nodes.at(1) == 0 ? 0 : newX(nodes.at(1)-1);
        currentVoltage = v1 - v2;

        //currentCurrent = currentVoltage * up->getConductance();

        up->updateVals(currentVoltage, 0, 1);
    }

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