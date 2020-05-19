#ifndef INPUTMODULE_INPUT_HPP
#define INPUTMODULE_INPUT_HPP

#include <iostream>
#include <string>
#include <unordered_map>

#include <circuit/circuit.hpp>
#include <component/enums.hpp>

using namespace std;

// this function will eventually read from file and convert to istream to pass into second overload
void readSpice(Circuit& c, string file);
// this function setsup component c from what it reads from istream
void readSpice(Circuit& c, istream& file);
// this function setsup basic variables for circuit
void setupBasic(Circuit& c, float timeStep);

//used for .model statements
struct ModelStatement{
    component component;
    string componentId;
    unordered_map<string, float> params;

    //string must be all uppercase
    static constexpr unordered_map<string, component> componentTable = {
        {"D", component::DIODE},
        {"NMOS", component::NMOS},
        {"PMOS", component::PMOS},
        {"NPN", component::NPN},
        {"PNP", component::PNP}
    }

    ModelStatement(vector<string> args);
};

#endif