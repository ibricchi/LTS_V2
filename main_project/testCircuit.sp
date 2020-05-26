Dependent source test
*
XU1 N001 N003 N002 IDEAL
R1 N002 N001 2k
R2 N001 0 1k
V1 N003 0 sin(0 20 10)
*
.tran 0.0001 .5 0 1
.end