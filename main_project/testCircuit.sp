Dependent source test
*
V1 N1 0 sin(0 1 10)
Q1 N2 N1 N3
V2 N4 0 0
R1 N2 N4 10k
R2 N3 0 3k
*
.tran 0.0001 0.5 0
.end