import numpy as np
from numpy import fft

def r_xx_dft(x,N=None):
    """
    Get the autocorrelation function of x.
    x is deterministic and therefore wide-sense stationary, so the
    autocorrelation function can be defined with a single argument.
    The resulting array R representing the autocorrelation function can be
    interpreted as looking up R(k) as follows:
    R[0:N/2] are for k = 0:N/2
    R[N/2+1:N] are for k = -N/2+1:0
    N is the number of autocorrelation values to compute, which is by default
    the length of x.
    """
    return fft.ifft(np.abs(fft.fft(x,n=N))**2)

