Dependent source test
*
V1 N001 0 10
R1 N002 N001 10k
C1 N002 0 100n
R2 N002 0 10k
*
.tran 0.0001 .2 0 1
.end