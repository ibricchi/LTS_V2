Dependent source test
*
V1 N001 0 sin(10 15 10)
D1 N001 N002
R1 N002 0 2k
C1 N002 0 100n
*
.tran 0.0001 .2 0 1
.end