Dependent source test
*
V1 N001 0 sin(0 1 10)
D1 N001 N002 asd
R1 N002 0 1k
*
.MODEL asd D (VT=0.026)
.tran 0.0001 1 0
.end