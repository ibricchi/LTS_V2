#ifndef OUTPUTMODULE_NONLINEARANALYSISTEST_HPP
#define OUTPUTMODULE_NONLINEARANALYSISTEST_HPP

#include <string>
#include <circuit/circuit.hpp>

void nonlinearSetup(Circuit& c);
string runNonlinearTransience(Circuit& c, float t);

#endif