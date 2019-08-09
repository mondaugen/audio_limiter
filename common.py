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

def hop_indicies(x,hop_size,window_size):
    n_hops=(len(x)-window_size)//hop_size
    n=np.arange(n_hops)*hop_size
    return n

def frame(x,hop_size,window_size):
    """
    A "frame" function independent of librosa.
    x must be one dimensional.
    windows are output in the colums, so that the whole resulting matrix can be
    left-multiplied to perform a transform.
    """
    hopi=hop_indicies(x,hop_size,window_size)
    n=np.add.outer(np.arange(window_size),hopi).astype('int')
    ret=x[n]
    return ret

