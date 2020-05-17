#ifndef OUTPUTMODULE_NONLINEARANALYSIS_HPP
#define OUTPUTMODULE_NONLINEARANALYSIS_HPP

#include <string>
#include <vector>
#include <circuit/circuit.hpp>
#include <component/component.hpp>

void nonLinearSetup(Circuit& c);
string runNonLinearTransience(Circuit& c, float t);

#endif