Dependent source test
*
V1 N003 0 10
V2 N004 0 3
D1 N002 N004 D
R1 N001 N002 1k
R2 N001 N003 4k
R3 N001 0 4k
*
.MODEL asd D (VT=0.026)
.tran 0.0001 .2 0
.end