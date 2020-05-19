#include <vector>
#include <string>
#include <circuit/circuit.hpp>
#include <Eigen/Dense>
#include <component/component.hpp>
#include <component/capacitor.hpp>
#include <component/inductor.hpp>

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

    float threshold = 0.1;

    VectorXf startX = c.getX();
    MatrixXf currentX = c.getX();
    MatrixXf newX = c.getX();

    // IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    // cout << newX.format(CleanFmt) << endl << endl;

    // keep calculating for current time step till threshold is bellow ceratin level
    int count = 0;
    int maxCount = 1;
    float gamma = 0.1;
    do{
        if(count > maxCount){
            gamma *= 0.9;
            c.setX(startX);
            currentX = c.getX();
            newX = c.getX();
            maxCount += 1;
        }
        c.nonLinearA();
        c.computeA_inv();
        c.nonLinearB();
        c.computeNLX(gamma);
        c.updateNodalVoltages();
        currentX = newX;
        newX = c.getX();

        // IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
        // cout << c.getA().format(CleanFmt) << endl << endl;
        // cout << c.getA_inv().format(CleanFmt) << endl << endl;
        // cout << c.getB().format(CleanFmt) << endl << endl;
        // cout << currentX.format(CleanFmt) << endl << endl;
        // cout << newX.format(CleanFmt) << endl << endl;

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
bool matrixDiffBellowThreshold(MatrixXf& m1, MatrixXf& m2, float d){
    for(int i = 0; i < m1.rows(); i++){
        //cout << m1(i) << " " << m2(i) << endl << endl;
        if(abs(m1(i) - m2(i)) > d){
            return false;
        }
    }
    return true;
}