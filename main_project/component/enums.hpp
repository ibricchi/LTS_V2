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
    nonLinear = 6
};
enum struct sourceTypes{
    DC = 0,
    SIN = 1
};
enum struct component{
    DIODE,
    NMOS,
    PMOS,
    NPN,
    PNP
}
// enum struct diodeParamTypes{
//     IS, //saturation current
//     N, //emission coefficient
//     RS, //series resistance
//     CJO, //junction capacitance
//     TT, //transit time
//     BV, //reverse bias breakdown voltage
//     IBV //reverse bias breakdown current
// };
// enum struct mosfetParamTypes{
//     L, //length
//     W, //width
//     VAF //early voltage
// };
// enum struct bjtParamTypes{
//     BF, //beta (common emitter current gain)
//     IS, //saturation current
//     VAF //early voltage
// };

#endif