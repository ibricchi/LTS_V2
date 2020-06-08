TEST CIRCUIT
*
Q1 N001 N002 0 0 NPN
V2 N002 0 SIN(0 1 10)
*
.model D D
.model PNP PNP
.model NPN NPN
.model NMOS NMOS
.model PMOS PMOS
.tran 0.00001 1 0
.end
