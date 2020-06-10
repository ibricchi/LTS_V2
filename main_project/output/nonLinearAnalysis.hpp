#ifndef OUTPUTMODULE_NONLINEARANALYSIS_HPP
#define OUTPUTMODULE_NONLINEARANALYSIS_HPP

#include <string>
#include <circuit/circuit.hpp>

void nonLinearSetup(Circuit& c, bool isDc = false);
string runNonLinearTransience(Circuit& c, double t, VectorXd& interpolX1, VectorXd& interpolX2, double& interpolT1, double& interpolT2,double& printTime,vector<float>& interpolI1, vector<float>& interpolI2);
bool matrixDiffBellowThreshold(VectorXd& m1, VectorXd& m2, float d);
void initializeDcBias(Circuit &c, int maxIterationsPerSourceStep = 5, float minimumStep = 1e-4, float threshold = 0.001);
bool hasNan(VectorXd& m);

#endif
