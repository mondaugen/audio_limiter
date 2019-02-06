import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

N=256
M=4096
x1=np.linspace(-1,1,N)
x2=np.concatenate((x1,x1))
x3=np.concatenate((x2,x2))
X1=np.fft.fft(x1,M)/N
X2=np.fft.fft(x2,M)/(2*N)
X3=np.fft.fft(x3,M)/(4*N)

fs=44100

f1=np.arange(M)/M*fs
fig,axs=plt.subplots(3,1)
axs[0].plot(f1[:len(f1)//2],20*np.log10(np.abs(X1[:len(f1)//2])))
axs[1].plot(f1[:len(f1)//2],20*np.log10(np.abs(X2[:len(f1)//2])))
axs[2].plot(f1[:len(f1)//2],20*np.log10(np.abs(X3[:len(f1)//2])))

plt.show()
