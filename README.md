# LTS_V2

All the code for LTS_V2 can be found in the main_project folder. Other folders were used to share reaserch, and testing scripts.

## Example Simulation

Circuit diagram:

![lts_v2_circuit](https://user-images.githubusercontent.com/53228351/178119791-153266aa-3dd0-43a2-adba-7695af10c053.png)

Simulation output for voltage source `V2`:

![lts_v2_simulation](https://user-images.githubusercontent.com/53228351/178119801-87275934-b4b9-499e-8bd1-46de29f5471a.png)

## Dependancies
To run LTS_V2, the Eigen3 C++ library and CMake must be installed.

To set up Eigen3, untar eigen-3.3.7.tar.bz2 from the dependancies folder and copy the "Eigen" sub-folder into the main_project folder.

Installalling CMake (Ubuntu): sudo apt-get install cmake

## Execution

To compile, simply run ./buildMain.sh inside the main_project folder. This will create an executable called LTS_V2.

To simulate a netlist, run: ./LTS_V2 < (path to netlist file)

By default, the simulation results will be written to a CSV file called out.csv.
The output file name can be changed by passing the new file name as a command line argument to the program.

## Netlist support

Netlist support is available as per official SPICE format.

Components that can be simulated:

* Resistor (R)
* Capacitor (C)
* Inductor (I)
* Voltage source (V)
* Current source (C)
* Voltage dependant voltage source (E)
* Current dependant current source (F)
* Votlage dependant current source (G)
* Current dependant voltage source (H)
* Ideal op-amp (X)
* Diode (D)
* BJT (Q)
* MOSFET (M)

The voltage and current source can be one of DC, SIN, PWL or Pulse

For non-linear components (Diode, BJT, MOSFET), .model statments can be used to adjust parameters

The .options command is available and provides support for ABSTOL, GMIN, and IMAX

Currently, only transienct analysis is supported: .tran (print step) (stop time) (start time) (max time step)

\* can be used for comment lines (They will be ignored by the netlist parser)

The firt netlist line indicates the circuit title and is ignored by the netlist parser

All netslists require the presence of a .END command to siginal the end of the netlist. All netlist lines after this statement will be ignored

## example netlist

Example Circuit

\* Comment example

R1 N003 N008 10k

XU1 N009 N003 N001 opamp Aol=100K GBW=10Meg

R3 N009 0 1k

R4 N009 N001 3k

R5 N001 0 1k

G1 N002 N003 N003 N008 10

R6 N002 N008 2k

R7 N004 0 10k

V1 N008 0 PWL(0 5 .1 3 .2 6 .4 10)

R8 N004 N001 1.5k

H1 N006 N005 V1 2.5

R9 N007 0 5k

R10 N007 N006 1k

C1 N003 0 1n

L1 N005 N004 1m

\*

.model NPN NPN

.model PNP PNP

.tran 1m 0.5

.options gmin=1p abstol=0.01 imax=1000

.END

