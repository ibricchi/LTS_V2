#include <vector>
#include <string>
#include <cmath>
#include <circuit/circuit.hpp>
#include <Eigen/Dense>
#include <component/component.hpp>
#include <component/voltageSource.hpp>
#include <component/inductor.hpp>
#include <component/capacitor.hpp>

#include "nonLinearAnalysis.hpp"

void nonLinearDCSetup(Circuit& c){
    c.setupCurrentControlledSources(c);
    c.nonLinearA(true);
    c.nonLinearB(true);
    c.computeA_inv();
    c.computeNLX(0);
    c.updateNodalVoltages();
}

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

    //forms a row in the csv file
    string outLine{};

    const float threshold = .001;

    // c.updateNodalVoltages();
    // VectorXd startX = c.getX();
    VectorXd currentX = c.getX();
    VectorXd newX = c.getX();

    // IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    // cout << newX.format(CleanFmt) << endl << endl;

    // keep calculating for current time step till threshold is bellow ceratin level
    int count = 0;
    int maxCount = 500;
    float gamma = 0.1;

    do{
        if(count > maxCount){
            cerr << "Newton Raphson count too big" <<endl;
            exit(1);
        }

        c.nonLinearA();
        c.computeA_inv();
        c.nonLinearB();
        c.computeNLX(gamma); //simply does A_inv*b (same as for linear x)
        currentX = newX;
        newX = c.getX();
        c.updateNodalVoltages(); //update based on newX

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

    //output current time 
    c.setCurrentTime(t); //Do we need this?
    outLine += to_string(t);

    //output node voltages
    for(int i{}; i<highestNodeNumber; i++){
        outLine += "," + to_string(newX(i));
    }

    //output component currents
    for(const auto &comp : components){
        outLine += "," + to_string(comp->getTotalCurrent(newX, highestNodeNumber));
    }

    // update components before next calculation of b
    for(const auto &comp : timeUpdatables){
        comp->updateVals(t+c.getTimeStep());
    }

    //update components based on current voltage/current
    float currentVoltage{}, currentCurrent{};
    for(const auto &up : vcUpdatables){
        auto nodes = up->getNodes();

        float v1 = nodes.at(0) == 0 ? 0 : newX(nodes.at(0)-1);
        float v2 = nodes.at(1) == 0 ? 0 : newX(nodes.at(1)-1);
        currentVoltage = v1 - v2;

        up->updateVals(currentVoltage, 0, 1);
    }

    return outLine;
    
}

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

void initializeDcBias(Circuit &c, int maxIterationsPerSourceStep, float minimumStep, float threshold){
    //DC bias is reached when Newton-Raphson converges and alpha is 1

    float alpha = 1; //factor that the source values are multiplied by
    float step = 0.1; //step that alpha is increases by (after firstConvergingAlpha has been found)
    float firstConvergingAlpha = nanf(""); //determined while decreasing alpha from 1 during first iteration
    VectorXd lastConvergingX;

    int count{};
    VectorXd startX = c.getX();
    VectorXd currentX = c.getX();
    VectorXd newX = c.getX();
    
    do{
        //reset count
        count = 0;

        do{
            cout << "count: " << count <<endl;
            cout << "step: " << step <<endl;
            cout << "alpha: " << alpha <<endl;

            //check if step becomes too small
            if(step < minimumStep){
                cerr << "The DC bias point could not be determined: Minimum step size reached." <<endl;
                exit(1);
            }

            //check for divergence 
            if(count > maxIterationsPerSourceStep){
                if(firstConvergingAlpha == nanf("")){
                    //if first convergence has not been found yet 
                    //=> decrease alpha and try again to find firstConvergingAlpha
                    alpha /= 2;

                    //reset circuit to starting values
                    currentX = startX;
                    c.setX(startX); //required for updateNodalVoltages to work
                    c.updateNodalVoltages();
                }else{
                    //if a firstConvergingAlpha already exists (step was too big)
                    //=> decrease step and try again
                    step /= 2;
                    alpha -= step; //reseting to last alpha and adding half old step = subtracting half old step from current alpha

                    //reset circuit to last converging values
                    currentX = lastConvergingX;
                    c.setX(lastConvergingX); //required for updateNodalVoltages to work
                    c.updateNodalVoltages();
                }
                count = 0;
                continue;
            }

            c.nonLinearA();
            c.computeA_inv();
            c.nonLinearB(alpha);
            c.computeNLX(0); //simply does A_inv*b (same as for linear x)
            currentX = newX;
            newX = c.getX();
            c.updateNodalVoltages(); //update based on newX

            count++;
        }
        while(!matrixDiffBellowThreshold(currentX, newX, threshold));

        //check if current convergence is first convergence
        if(firstConvergingAlpha == nanf("")){
            firstConvergingAlpha = alpha;
        }

        //save current x as last converging x
        lastConvergingX = newX;

        //step up sources
        alpha += step;
    }while(alpha < 1);

    //initialize capacitors/inductors to DC bias point
    auto vcUpdatables = c.getVCUpdatablesRef();
    int numberOfInductors = c.getInductorNumber();
    int whichInductor = 0; //Starts count from zero, so if whichInductor == 3, it's on the 4th inductor, variable shows which inductor is being referred to in the following for    
    for(const auto &up : vcUpdatables){
        auto nodes = up->getNodes();

        float v1 = nodes.at(0) == 0 ? 0 : newX(nodes.at(0)-1);
        float v2 = nodes.at(1) == 0 ? 0 : newX(nodes.at(1)-1);
        float currentVoltage = v1 - v2;

        cout <<endl<<endl << "newX: " <<endl;
        cout << newX;
        cout <<endl<<endl;

        //These next lines initialise the compCurrent values of capacitors and inductors based on DC bias values of voltage and current
        if(typeid(*up)==typeid(Capacitor)){
            up->initCompCurrent(currentVoltage);
        }else if(typeid(*up)==typeid(Inductor)){
            up->initCompCurrent(newX[newX.size()-numberOfInductors+whichInductor]);
            whichInductor++;
        }	
		
        //Need to call getTotalCurrent before updateVals
        up->getTotalCurrent(newX, c.getHighestNodeNumber());
        up->updateVals(currentVoltage, 0, 1);
    }
}