#  generate data for testing y[n] = y[n] * (1 - x[n])

import numpy as np
import os
from gen_common import complete_array

NVALS=1000

x=np.random.standard_normal((NVALS,))
y=np.random.standard_normal((NVALS,))
y_r=y*(1-x)

s=""

s+="""
#define NVALS %d

""" % (NVALS,)

s+="""
static float y_result[] = {
"""

s=complete_array(y_r,s)

s+="""
static float y[] = {
"""

s=complete_array(y,s)

s+="""
static float x[] = {
"""

s=complete_array(x,s)

outfile=os.environ['OUTFILE']
with open(outfile,"w") as f:
    f.write(s)
