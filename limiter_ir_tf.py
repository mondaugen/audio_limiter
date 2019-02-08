import numpy as np
import scipy
from scipy import signal

# A limiter whose transfer function is computed as the impulse response of a
# low-order filter. This way we don't need to store values of the transfer
# function far into the future, saving time and space.

class filter_w_ir:
    # A filter whose IR is stored with it
    def __init__(self,
    b, # b coefficients
    a, # a coefficients
    m, # number of samples of IR to compute and store
    ):
        self.b = b
        self.a = a
        self.m = m
        self.ir = np.zeros((m,))
        self.ir[0] = 1
        self.ir = signal.lfilter(self.b,self.a,self.ir)
    def filter(self,x,x_past,y_past):
        # x_past is [x[-N],x[1-N],....x[-1]] where 0 is now
        # y_past is [y[-N],y[1-N],....y[-1]]
        z_i=signal.lfiltic(self.b,self.a,np.reverse(y_past),np.reverse(x_past))
        return signal.lfilter(self.b,self.a,x,zi=z_i)

        
