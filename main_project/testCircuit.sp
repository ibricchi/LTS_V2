Dependent source test
*
R1 N001 N002 10k
R2 N004 0 3k
V1 N003 0 SINE(1 5 10)
V2 N001 0 20
M1 N002 N003 N004 N004 asd
*
.model asd NMOS (VTO=2.9 K=0.25mV VA=90V)
.tran 0.0001 0.5 0
.end