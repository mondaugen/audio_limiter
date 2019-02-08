# Find the sum of 2 exponentials that satisfy some constraints in a least-squares
# sense
# Use log sum of exp for better precision

# Conclusions:
# I think no solution exists for many configurations of n_p and n_d so this
# cannot hope to find coefficients giving these values
# Really, as long as the
# peak is close to n_p and the n_p is not so different, then probably the
# transfer functions will sound ok.  But the problem is that the transfer
# function still needs to be inspected by hand BUT probably this the best
# transfer function you will get only using a filter of order 2, so you should
# just accept the answer the computer gives Also the solver still seems to try
# infeasible points, but still succeeds when there is a solution, so this is
# weird (bug?)

import matplotlib.pyplot as plt
import numpy as np
import scipy
import pdb
from scipy import optimize
import warnings
#warnings.filterwarnings('error')

def log_sum_exp(x,b):
    # compute log(sum(exp(x))) but with improved precision for large x
    a=np.max(x)
    try:
        return a+np.log(np.sum(b*np.exp(x-a)))
    except RuntimeWarning:
        pdb.set_trace()

def obj_fun(x,n_p,n_d,max_w):
    # n_p is the sample index of the peak
    # n_d is the sample index of the -60 dB point
    # max_w is the relative weighting of fitting the maxmimum versus the decay value
    b=x
    u=0
    # f'(n_p) = 0
    try:
        u+=max_w*(np.log(-b[0])-np.log(-b[1])-(b[1]-b[0])*n_p)**2
    except RuntimeWarning:
        pdb.set_trace()
    # f(n_d) = f(n_p)*1e-3
    u+=(log_sum_exp(b*n_d,np.array([1,-1]))
        -log_sum_exp(b*n_p,np.array([1,-1]))
        -np.log(1e-3))**2
    return np.array([u])

def grid_search(n_p,n_d,max_w,N_points=10):
    res=[]
    arg_res=[]
    b_min=-0.1
    for b0 in b_min/np.power(2,1+np.arange(N_points)):
        for b1 in (b_min-b0)/np.power(2,1+np.arange(N_points))+b0:
            #print([b0,b1])
            b=np.array([b0,b1])
            res.append(obj_fun(b,n_p,n_d,max_w)[0])
            arg_res.append(b)
        #print()
    min_res_i=np.argmin(res)
    return arg_res[min_res_i],res[min_res_i]

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
    n=np.arange(n_d+1)
    y=np.exp(b[0]*n)-np.exp(b[1]*n)
    y_max=np.max(y)
    y_arg_max=np.argmax(y)
    y/=y_max
    print("Filter coeffs: " + str(np.exp(b)))
    print("Value at n_p: %f" % (20*np.log10(y[n_p]),))
    print("Value at n_d: %f" % (20*np.log10(y[n_d]),))
    print("Location of peak: %d" % (y_arg_max,))
    print("Desired peak: %d" % (n_p,))
    print("n_d: %d" % (n_d,))
    plt.plot(n,20*np.log10(y))

M=2
n_p=300
n_d=64000
max_w=1

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
    return -1*A.dot(x)+np.array([-1e-6,-1e-6,-1e-8])

def jac_constr(x):
    A=np.array([
        [1,0],
        [0,1],
        [-1,1]
    ])
    return -1*A

# Find starting point using grid search
b,r=grid_search(n_p,n_d,max_w,N_points=10)
print("starting b "+str(b))
print("starting cost "+str(r))
result=optimize.minimize(
obj_fun,
b,
args=(n_p,n_d,max_w),
jac=jac_obj_fun,
method='SLSQP',
constraints=[dict(
type='ineq',
fun=constr,
jac=jac_constr)])

print("Success: %s" % (str(result.success),))
print(result.x)
print(result.fun)
print(result.message)
plot_result(result.x,n_p,n_d)
plt.show()
