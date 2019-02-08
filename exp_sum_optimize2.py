# Find the sum of 2 exponentials that satisfy some constraints in a least-squares
# sense
# Use log sum of exp for better precision

import matplotlib.pyplot as plt
import numpy as np
import scipy
from scipy import optimize

def log_sum_exp(x,b):
    # compute log(sum(exp(x))) but with improved precision for large x
    a=np.max(x)
    return a+np.log(np.sum(b*np.exp(x-a)))

def obj_fun(x,n_p,n_d,max_w):
    # n_p is the sample index of the peak
    # n_d is the sample index of the -60 dB point
    # max_w is the relative weighting of fitting the maxmimum versus the decay value
    b=x
    u=0
    # f'(n_p) = 0
    u+=max_w*(np.log(-b[0])-np.log(-b[1])-(b[1]-b[0])*n_p)**2
    # f(n_d) = f(n_p)*1e-3
    u+=(log_sum_exp(b*n_d,np.array([1,-1]))
        -log_sum_exp(b*n_p,np.array([1,-1]))
        -np.log(1e-3))**2
    return np.array([u])

def jac_obj_fun(x,n_p,n_d,max_w):
    b=x
    D0=max_w*2*(np.log(-b[0])-np.log(-b[1])-(b[1]-b[0])*n_p)
    D1=2*(log_sum_exp(b*n_d,np.array([1,-1]))
        -log_sum_exp(b*n_p,np.array([1,-1]))
        -np.log(1e-3))
    d_b0=D0*(1/b[0]+n_p)
    d_b1=D0*(-1/b[1]-n_p)
    d_b0+=D1*(n_d/(1-np.exp((b[1]-b[0])*n_d))
        - n_p/(1-np.exp((b[1]-b[0])*n_p)))
    d_b1+=D1*(-n_d/(1-np.exp((b[0]-b[1])*n_d))
        + n_p/(1-np.exp((b[0]-b[1])*n_p)))
    return np.array([d_b0,d_b1])

def plot_result(x,n_p,n_d):
    b=x
    n=np.arange(n_d)
    y=np.exp(b[0]*n)-np.exp(b[1]*n)
    plt.plot(n,y)

M=2
n_p=100
n_d=1000

#constr=optimize.LinearConstraint(
#np.array([
#    [1,0],
#    [0,1],
#    [-1,1]
#]),
#-1*np.inf*np.ones(3),
#np.zeros(3))

def constr(x):
    A=np.array([
        [1,0],
        [0,1],
        [-1,1]
    ])
    # the explanation of the array added at the end is follows:
    # first value means b0 never greater than it (should be non-positive)
    # second value means b1 never greater than it (should be non-positive)
    # last value means b1-b0 less than it
    return -1*A.dot(x)+np.array([-1e-3,-1e-3,-1e-4])

b0=-0.01*np.random.ranf()
b1=-0.01*np.random.ranf()+b0
result=optimize.minimize(
obj_fun,
np.array([b0,b1]),
args=(n_p,n_d,100),
jac=jac_obj_fun,
constraints=[dict(
type='ineq',
fun=constr)])

print("Success: %s" % (str(result.success),))
print(result.x)
print(result.fun)
print(result.message)
plot_result(result.x,n_p,n_d)
plt.show()
