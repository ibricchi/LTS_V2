Inverting Op-Amp from EE1 Term 2 Labs
V1 N001 N003 SINE(0 3 10)
D1 0 N001 D
D2 0 N003 D
D3 N001 N002 D
D4 N003 N002 D
R1 N002 0 100k
C1 N002 0 50µ
.model NPN NPN
.model PNP PNP
.tran 10u 1
*.options gmin=1p abstol=0.1 imax=1000
.END
