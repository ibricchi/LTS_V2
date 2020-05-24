Simple Diode
*
V1 N2 0 SIN(0 1 10)
V2 N3 0 5
R1 N3 N2 1k
Q1 N3 N2 N1
R2 N1 0 1k
*
.tran 0.1 1 0 0.000001
.end
