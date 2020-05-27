Dependent source test
*
V1 N001 0 12
R1 N001 N003 1k
R2 N002 0 1k
R3 N003 0 1k
F1 N003 N002 V1 2
*
.tran 0.0001 .5 0 1
.end