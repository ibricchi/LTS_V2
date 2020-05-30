Dependent source test
*
V1 N001 0 5
R1 N002 N001 10k
D1 N003 N002
C1 N003 0 1n
R2 N003 0 5k
*
.tran 0.0001 .2 0 1
.end