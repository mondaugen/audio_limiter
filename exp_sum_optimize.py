# Find the sum of exponentials that satisfy some constraints in a least-squares
# sense

import matplotlib.pyplot as plt
import numpy as np
import scipy
from scipy import optimize


def obj_fun(x,n_p,n_d):
    # n_p is the sample index of the peak
    # n_d is the sample index of the -60 dB point
    # M exponentials
    M=len(x)//2
    a=x[:M]
    b=x[M:]
    # f(0) = 0
    u=np.sum(a)**2
    # f(n_p) = 1
    u+=(np.sum(a*np.exp(b*n_p))-1)**2
    # f(n_d) = 1e-3
    #u+=(np.sum(a*np.exp(b*n_d))-1e-3)**2
    # f'(n_p) = 0
    u+=np.sum(b*a*np.exp(b*n_p))**2
    return np.array([u])

def jac_obj_fun(x,n_p,n_d):
    M=len(x)//2
    a=x[:M]
    b=x[M:]
    D0=2*np.sum(a)
    D1=2*(np.sum(a*np.exp(b*n_p))-1)
    D2=2*(np.sum(a*np.exp(b*n_d))-1e-3)
    D3=2*np.sum(b*a*np.exp(b*n_p))
    d_da=D0+D1*np.exp(b*n_p)+D2*np.exp(b*n_d)+D3*b*np.exp(b*n_p)
    #d_da=D0+D1*np.exp(b*n_p)+D3*b*np.exp(b*n_p)
    d_db=D1*n_p*a*np.exp(b*n_p)+D2*n_d*a*np.exp(b*n_d)+D3*(a+b*b*a)*np.exp(b*n_p)
    #d_db=D1*n_p*a*np.exp(b*n_p)+D3*(a+b*b*a)*np.exp(b*n_p)
    return np.concatenate((d_da,d_db))

def plot_result(x,n_p,n_d):
    M=len(x)//2
    a=x[:M]
    b=x[M:]
    n=np.arange(n_d)
    y=a[:,None]*np.exp(np.multiply.outer(b,n))
    y=np.sum(y,axis=0)
    #y=-10*np.exp(-0.01*n)+10*np.exp(-0.0008*n)
    plt.plot(n,y)

M=2
n_p=100
n_d=1000

result=optimize.minimize(
obj_fun,
np.concatenate(((np.random.ranf((M,))-0.5)*2,-0.1*np.random.ranf((M,)))),
args=(n_p,n_d),
#method='CG',
jac=jac_obj_fun)

print("Success: %s" % (str(result.success),))
print(result.x)
print(result.fun)
print(result.message)
plot_result(result.x,n_p,n_d)
plt.show()
