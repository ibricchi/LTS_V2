Dependent source test
*
V1 N001 0 SINE(2 5 10 .1 5 90)
R1 N001 0 1k
*
.MODEL asd D (VT=0.026)
.tran 0.0001 .5 0
.end