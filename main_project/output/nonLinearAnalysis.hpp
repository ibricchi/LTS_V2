#ifndef OUTPUTMODULE_NONLINEARANALYSIS_HPP
#define OUTPUTMODULE_NONLINEARANALYSIS_HPP

#include <string>
#include <circuit/circuit.hpp>

void nonLinearSetup(Circuit& c);
string runNonLinearTransience(Circuit& c, float t);
bool matrixDiffBellowThreshold(VectorXd& m1, VectorXd& m2, float d);
void initializeDcBias(Circuit &c, int maxIterationsPerSourceStep = 20, float minimumStep = 0.001, float threshold = 0.001);

#endif