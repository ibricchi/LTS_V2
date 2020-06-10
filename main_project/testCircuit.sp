Inverting Op-Amp from EE1 Term 2 Labs
V1 N001 0 SINE(0 5 10)
D1 N002 N001 D
R1 N002 0 10k
.model NPN NPN
.model PNP PNP
.tran 10u 1
*.options gmin=1p abstol=0.1 imax=1000
.END
