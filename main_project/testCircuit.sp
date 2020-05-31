Dependent source test
*
V1 n1 0 10
V2 n3 0 3
D1 n2 n3 D
R1 N001 n2 1k
R2 N001 n1 4k
R3 N001 0 4k
*
.MODEL asd D (VT=0.026)
.tran 0.0001 1 0
.end