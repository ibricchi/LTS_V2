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

/*
    Used for .model statements.
    .model statement syntax example => '.model D1N4148 D (is=1n N=1.2)'
    The .model statement, param names (is, n) and the componentName (D) are case insensitive.
    The componentId (D1N4148) must correspond exactly to the modelName of a component (case sensitive).
    The spaces are important (Cannot remove or add extra spaces).
*/
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
        {"TYPE", mosfetParamType::TYPE},
        {"K", mosfetParamType::K},
        {"VT", mosfetParamType::VT},
        {"VA", mosfetParamType::VA}
    };
    const unordered_map<string, bjtParamType> bjtParamTable = {
        {"BF", bjtParamType::BF},
        {"IS", bjtParamType::IS},
        {"VAF", bjtParamType::VAF}
    };
};

#endif