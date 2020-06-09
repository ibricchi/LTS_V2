#ifndef COMPONENTENUMS
#define COMPONENTENUMS

/*
    A collection of enums used in this project
*/

enum struct componentType{
    conductanceSource = 0,
    voltageSource = 1, 
    currentSource = 2,
    vcUpdatable = 3,
    timeUpdatable = 4,
    nonVoltageSource = 5,
    nonLinear = 6
};
enum struct sourceTypes{
    DC = 0,
    SIN = 1,
    PWL = 2,
    PULSE = 3
};
enum struct component{
    DIODE,
    NMOS,
    PMOS,
    NPN,
    PNP
};

//also need to add the params below to input/input.hpp/ModelStatement lookup tables
enum struct diodeParamType{
    IS, //saturation current
    N, //emission coefficient
    VTO
    // RS, //series resistance
    // CJO, //junction capacitance
    // TT, //transit time
    // BV, //reverse bias breakdown voltage
    // IBV //reverse bias breakdown current
};
enum struct mosfetParamType{
    TYPE,
    K, // transconductance
    ISat, // saturation current
    VTO, // theshold voltage
    VA, // early voltage
};
enum struct bjtParamType{
    TYPE,
    BF, //beta (common emitter current gain)
    BR, // beta reverse
    IFS, // forward saturation current
    IRS, // reverse saturation current
    VAF, //early voltage
    VT // Thermal Voltage
};

#endif