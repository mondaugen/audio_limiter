# Use Prony's method to model a transfer function and implement it as a filter

import common
import numpy as np
import scipy
from scipy import signal
import matplotlib.pyplot as plt

# The value which is considered to be 0 on the exponentially decaying curve
DECAY_MIN=1e-3

def prony(x,p):
    # We make the length of the autocorrelation function computed a power of 2
    # so we can wrap values of p easily
    N=1
    while N < (2*p):
        N = N << 1
    N_mask = N - 1
    r=common.r_xx_dft(x,N)
    k_l=np.add.outer(np.arange(p),-np.arange(p)) & N_mask
    R=r[k_l]
    a=np.linalg.solve(R,-r[1+np.arange(p)])
    b0=r[0]+np.sum(a*np.conj(r[1:p+1]))
    return a,b0

# Deterministic version of prony where we assume x is finite and use its finite
# autocorrelation sequence
def prony_det(x,p):
    N=len(x)
    x_=np.concatenate((np.zeros(p-1,dtype=x.dtype),x))
    # rows of X_i start with 0,1, ... N-2
    X_i=np.add.outer(np.arange(N-1),p-1-np.arange(p))
    X_q=x_[X_i]
    x_q=x[1:]
    print(X_q.shape)
    print(x_q.shape)
    a=np.linalg.lstsq(X_q,-x_q)[0]
    b0=x[0]
    return a,b0


def model_limiter_tf(
ramp_up=100,
hold_time=100,
ramp_down=5000,
p=2):
    s_=np.concatenate((
            np.arange(1,ramp_up+1)/ramp_up,
            np.ones(hold_time-1),
            np.power(np.power(DECAY_MIN,1/ramp_down),
                np.arange(ramp_down))))
    s_len=int(len(s_)*1.5)
    s=np.zeros(s_len,dtype=s_.dtype)
    s[:len(s_)]=s_
    print(s.shape)
    a,b=prony_det(s,p)
    x=np.zeros_like(s)
    x[0]=1
    y=signal.lfilter([b],np.concatenate(([1],a)),x)
    return s,y

s,y=model_limiter_tf(
ramp_up=100,
ramp_down=100000,
p=2)
fig,axs=plt.subplots(1,1)
axs.plot(np.arange(len(s)),s,label='ideal')
axs.plot(np.arange(len(y)),np.abs(y),label='model')
axs.legend()
plt.show()



#
#s=np.array([1,2,3,3,3,2,1,0,0,0,0])
#a,b=prony(s,16)
#x=np.zeros_like(s)
#x[0]=1
#y=signal.lfilter([b],np.concatenate(([1],a)),x)
#print(s)
#print(np.abs(y))
