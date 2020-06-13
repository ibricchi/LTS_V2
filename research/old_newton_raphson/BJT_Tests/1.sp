1
V1 0 1 SIN(0 1 10)
Q1 2 1 0 NPN
R1 2 3 1k
V2 3 0 1
.model NPN NPN
.model PNP PNP
.model D D
.lib C:\Users\IBricchi\Documents\LTspiceXVII\lib\cmp\standard.bjt
.tran 0 1 0
.backanno
.end
