#ifndef OUTPUTMODULE_NONLINEARANALYSIS_HPP
#define OUTPUTMODULE_NONLINEARANALYSIS_HPP

#include <string>
#include <circuit/circuit.hpp>

void nonLinearSetup(Circuit& c);
string runNonLinearTransience(Circuit& c, float t);
bool matrixDiffBellowThreshold(MatrixXf& m1, MatrixXf& m2, float d);

#endif