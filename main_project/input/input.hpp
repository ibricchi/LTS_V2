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
    component componentName;
    string componentId;
    unordered_map<int, float> params; //mapping of int enum to float

    ModelStatement(vector<string> args);

    //strings must be all uppercase
    const unordered_map<string, component> componentTable = {
        {"D", component::DIODE},
        {"NMOS", component::NMOS},
        {"PMOS", component::PMOS},
        {"NPN", component::NPN},
        {"PNP", component::PNP}
    };
    const unordered_map<string, diodeParamType> diodeParamTable = {
        {"IS", diodeParamType::IS},
        {"N", diodeParamType::N}
    };
    const unordered_map<string, mosfetParamType> mosfetParamTable = {
        {"L", mosfetParamType::L},
        {"W", mosfetParamType::W},
        {"VAF", mosfetParamType::VAF}
    };
    const unordered_map<string, bjtParamType> bjtParamTable = {
        {"BF", bjtParamType::BF},
        {"IS", bjtParamType::IS},
        {"VAF", bjtParamType::VAF}
    };
};

#endif