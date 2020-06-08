TEST CIRCUIT
*
V1 N001 0 SIN(0 3 10)
D1 N001 N002 D
R1 N002 0 1k
*
.model D D
.model PNP PNP
.model NPN NPN
.model NMOS NMOS
.model PMOS PMOS
.tran 0.00001 1 0
.end
