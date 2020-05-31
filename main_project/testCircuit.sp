Dependent source test
*
V1 N001 0 sin(10 15 10)
M1 N001 N002 N003 asd
R2 N003 0 1k
R1 N002 0 2k
*
.MODEL asd NMOS (VT=0)
.tran 0.0001 .2 0 1
.end