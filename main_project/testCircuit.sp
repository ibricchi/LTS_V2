Dependent source test
*
V1 N001 0 12
R1 N001 N003 1k
R2 N002 0 1k
R3 N003 0 1k
H1 N002 N003 V1 2
*R4 N2 N3 1k
*
.tran 0.0001 .5 0 1
.end