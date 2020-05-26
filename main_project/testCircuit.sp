Dependent source test
*
V1 N001 0 12
R1 N001 N003 1k
R2 N002 0 1k
R3 N003 0 1k
E1 N002 N003 N001 N003 2
*
*Diode is not connected to the rest of the circuit
*It is just here to test the dependent sources in a nonlinear simulation
D1 N4 N4 
*
.tran 0.0001 .5 0 1
.end