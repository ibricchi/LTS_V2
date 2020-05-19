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
    nonLinear = 5
};

enum struct sourceTypes{
    DC = 0,
    SIN = 1
};

enum struct mosfetType{
    NMOS = 0,
    PMOS = 1
};

enum struct operatingRegion{
    CUTOFF = 0,
    TRIODE = 1,
    SATURATION = 2
};

#endif