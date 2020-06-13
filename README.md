# LTS_V2

All the code for LTS_V2 can be found in the main_project folder. Other folders were used to share reaserch, and testing scripts.

## Dependancies
To run the program EIGEN library and CMake must available.

If Eigen is not set up on the system, to set up untar eigen tar from dependancies folder and copy "Eigen" sub-folder into main_project folder

Also need to install CMake: sudo apt-get install cmake

## Execution

To compile simply run buildMain.sh. This will create an executable called LTS_V2

To simulate a netlist run: ./LTS_V2 < (path to file)

This will create a csv file out.csv with results.

## Netlist support

Netlist support is available as per official spice format.

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
* Op amp (X)
* Diode (D)
* BJT (Q)
* Mosfet (M)

For non-linear components (Diode BJT Mosfet) .model statments can be used to change parameters

.option command is available and has support for ABSTOL, GMIN, and IMAX

Currently only transienct analysis is supported .tran (print step) (stop time) (start time) (max time step)

\* can be used for comments

Firt line of netlist indicates circuit title

All netslist require the presence of a .END command to siginal the end of the file

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

.tran 1m .5

.options gmin=1p abstol=0.01 imax=1000

.END

