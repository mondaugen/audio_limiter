import numpy as np
from scipy import signal
from gen_common import complete_array
import os


r=0.999
b=[r,-r]
a=[1,-r]

NVALS=1007
DC=1

x=np.random.standard_normal((NVALS,))+DC
x=x.astype('float32')
y=signal.lfilter(b,a,x)

s=""

s+="""
#define NVALS %d
static const float r = %.18f;
""" % ((NVALS,r))

s+="""
static const float x[] = {
"""
s=complete_array(x,s)

s+="""
static const float y[] = {
"""
s=complete_array(y,s)

outfile=os.environ['OUTFILE']
with open(outfile,"w") as f:
    f.write(s)
