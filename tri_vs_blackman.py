import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

def shift(x,n):
    n0=n
    while n0 < 0:
        n0 += len(x)
    while n0 >= len(x):
        n0 -= len(x)
    y=np.zeros_like(x)
    y[:len(x)-n0] = x[n0:]
    y[len(x)-n0:] = x[:n0]
    return y

ramp_up=256
n_shifts=[-10,-30,-100,-300,-1000]
N=4096
x=np.zeros((N,))
x[:2*ramp_up] = signal.windows.blackmanharris(2*ramp_up)
sr=44100
f=np.arange(N)/N*sr
fig,axs=plt.subplots(2,1)
for n_shift in n_shifts:
    y = shift(x,n_shift)
    z=np.where(x > y,x,y)
    #z = shift(x,2*n_shift)
    #x=np.where(x > z,x,z)
    X=np.fft.fft(z)/N # N is integral of rectangular window
    axs[0].plot(np.arange(N),z,label=str(n_shift))
    axs[1].plot(f[:N//8],20*np.log10(np.abs(X[:N//8])),label=str(n_shift))
plt.legend()
plt.show()

