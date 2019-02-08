import numpy as np
from numpy import fft

x=np.array([0,1,2,3,0,0,0,0])

def r_xx(x,k):
    N = len(x)
    if abs(k) > N:
        return 0
    x_k = np.zeros(N+k,dtype=x.dtype)
    if k < 0:
        x_k[:N+k]=x[-k:N]
        x_ = x[:N+k]
    else:
        x_k[k:N+k]=x
        x_=np.zeros_like(x_k)
        x_[:N]=x
    return sum(np.conj(x_)*x_k)

def r_xx_dft(x,N=None):
    return fft.ifft(np.abs(fft.fft(x,n=N))**2)

print([r_xx(x,k) for k in range(-4,4)])
print(np.abs(r_xx_dft(x,16)))
