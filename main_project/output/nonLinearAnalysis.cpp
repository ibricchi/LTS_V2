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

void nonLinearSetup(Circuit& c, bool isDc){
    c.setupCurrentControlledSources(c); //add idx of the controlling voltage source (must come prior to setting up A)
    c.nlSetup(isDc);
}

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

    VectorXd currentX = c.getX();
    VectorXd newX = c.getX();

    // keep calculating for current time step till threshold is bellow ceratin level
    int count = 0;
    int maxCount = 100;
    // float gamma = 0.1;

    do{
        if(count > maxCount){
            cerr << "Newton Raphson count too big" <<endl;
            exit(1);
        }

        c.nonLinearA();
        c.computeA_inv();
        c.nonLinearB();
        c.computeNLX(0); //simply does A_inv*b (same as for linear x)
        currentX = newX;
        newX = c.getX();
        c.updateNodalVoltages(); //update based on newX

        count++;

        if(t > 0.009400){
            IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
            cout << "Time:" << t << endl;
            cout << "-----------------------" << endl;
            cout << c.getA().format(CleanFmt) << endl << endl;
            cout << c.getB().format(CleanFmt) << endl << endl;
            cout << c.getX().format(CleanFmt) << endl << endl;
        }
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
        outLine += "," + comp->getTotalCurrentString(newX, highestNodeNumber);
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

        //Need to call getTotalCurrentString before updateVals
        up->updateVals(currentVoltage, 0, 1);
    }

    return outLine;
    
}

// both matrixes are assumed to be x:1 matrixes with same x
bool matrixDiffBellowThreshold(VectorXd& m1, VectorXd& m2, float d){
    for(int i = 0; i < m1.rows(); i++){
        if((abs(m1(i) - m2(i)) > d) || isnan(m1(i)) || isnan(m2(i))){
            return false;
        }
    }
    return true;
}

void initializeDcBias(Circuit &c, int maxIterationsPerSourceStep, float minimumStep, float threshold){
    //DC bias is reached when Newton-Raphson converges and alpha is 1

    float alpha = 1; //factor that the source values are multiplied by
    float step = 0.05; //step that alpha is increases by (after firstConvergingAlpha has been found)
    float firstConvergingAlpha = -1; //determined while decreasing alpha from 1 during first iteration
    VectorXd lastConvergingX;

    int count{};
    VectorXd startX = c.getX();
    VectorXd currentX = c.getX();
    VectorXd newX = c.getX();
     
    do{
        //reset count
        count = 0;

        do{
            //check if step becomes too small
            if(step < minimumStep){
                cerr << "The DC operating point could not be determined: Minimum step size reached." <<endl;
                exit(1);
            }

            //check for divergence 
            if(count >= maxIterationsPerSourceStep || hasNan(newX)){
                if(firstConvergingAlpha == -1){
                    if(alpha == 0){
                        cerr << "The DC bias operating point could not be determined: Alpha cannot become smaller than 0." <<endl;
                        exit(1);
                    }

                    //if first convergence has not been found yet 
                    //=> decrease alpha and try again to find firstConvergingAlpha
                    alpha /= 2;

                    //reset circuit to starting values
                    currentX = startX;
                    newX = startX;
                    
                    c.setX(startX); //required for updateNodalVoltages to work
                    c.updateNodalVoltages();
                }else{
                    //if a firstConvergingAlpha already exists (step was too big)
                    //=> decrease step and try again
                    step /= 2;
                    alpha -= step; //reseting to last alpha and adding half old step = subtracting half old step from current alpha

                    //reset circuit to last converging values
                    currentX = lastConvergingX;
                    newX = lastConvergingX;
                    c.setX(lastConvergingX); //required for updateNodalVoltages to work
                    c.updateNodalVoltages();
                }
                count = 0;
                continue;
            }

            c.nonLinearA(true);
            c.computeA_inv();
            c.nonLinearB(true, alpha);
            c.computeNLX(0); //simply does A_inv*b (same as for linear x)
            currentX = newX;
            newX = c.getX();
            c.updateNodalVoltages(); //update based on newX

            count++;
        }
        while(!matrixDiffBellowThreshold(currentX, newX, threshold));

        //check if current convergence is first convergence
        if(firstConvergingAlpha == -1){
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

        //These next lines initialise the compCurrent values of capacitors and inductors based on DC bias values of voltage and current
        if(typeid(*up)==typeid(Capacitor)){
            up->initCompCurrent(currentVoltage);
        }else if(typeid(*up)==typeid(Inductor)){
            up->initCompCurrent(newX[newX.size()-numberOfInductors+whichInductor]);
            whichInductor++;
        }	
		
        //Need to call getTotalCurrentString before updateVals
        up->getTotalCurrentString(newX, c.getHighestNodeNumber());
        up->updateVals(currentVoltage, 0, 1);
    }
}

bool hasNan(VectorXd& m){
    // IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    // cout << m << endl;
    for(int i = 0; i < m.size(); i++){
        // cout << m(i) << endl;
        if(isnan(m(i))){
            // cout << "failed test" << endl;
            return true;
        }
    }
    return false;
}