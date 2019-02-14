# So instead of using complicated optimization techiniques to design an
# attenuation curve, we can just graft an arbitrary ramp (maybe raised cosine or
# gaussian) onto an exponentially decaying curve and have the best of both
# worlds.

import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

def raised_cos(n,n_p):
    return 1-0.5*(1+np.cos(np.pi*n/n_p))

def atn_fun_design_cos(n_p,n_d):
    # note the actual peak will be a bit before n_p
    a=np.power(1e-3,1/n_d)
    a_np=np.power(a,n_p)
    n=np.arange(n_d)
    r_unscaled=raised_cos(n,n_p)
    r=a_np*r_unscaled
    l=np.clip(np.arange(n_d)/n_p,0,1)
    a_n=np.power(a,n)
    w=(1-l)*r+l*a_n
    return w,r,a_n,r_unscaled

def atn_fun_design_cos_filter(n_p,n_d):
    # note the actual peak will be a bit before n_p
    # this uses a filter to render the exponential
    assert(n_p>=1)
    a=np.power(1e-3,1/n_d)
    a_np=np.power(a,n_p)
    n=np.arange(n_p)
    r=a_np*raised_cos(n,n_p)
    l=np.clip(n/n_p,0,1)
    a_n=np.power(a,n)
    w_=(1-l)*r+l*a_n
    w_max=np.max(w_)
    n_max=np.argmax(w_)
    ramp=(1-l)*r/w_max+l*a_n/w_max
    zi=signal.lfiltic([1],[1,-a],ramp[-1:])
    x=np.zeros((n_d-n_p,))
    dec,_=signal.lfilter([1],[1,-a],x,zi=zi)
    w=np.concatenate((ramp,dec))
    # returns "window", b filter coefficients, a filter coefficients
    return w,[1],[1,-a],n_max

if __name__ == '__main__':
    w,_,_,_=atn_fun_design_cos_filter(1000,250000)
    plt.plot(np.arange(len(w)),20*np.log10(w),label='blend')
    plt.legend()
    plt.show()
