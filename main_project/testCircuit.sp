Simple FET
*
V1 N3 0 SIN(0 5 5)
V2 N1 0 20
R1 N1 N2 1k
M1 N2 N3 0 nm
*
.model nm NMOS (VT=0)
*
.tran 0.0001 1 0 0.0001
.end