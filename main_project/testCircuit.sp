Inverting Op-Amp from EE1 Term 2 Labs
V1 N001 0 SINE(0 1 10)
D1 N001 N002 D
C1 N002 0 1m
R1 N002 0 10k
.model NPN NPN
.model PNP PNP
.tran 1m 1
*.options gmin=1p abstol=0.1 imax=1000
.END
