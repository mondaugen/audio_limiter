# Generate what the one-pole filter should output

import numpy as np
import os
from gen_common import complete_array

a=0.99

NVALS=1000

y=np.power(a,np.arange(NVALS))

s=""

s+="""
#define LEN_YN_TRUE %d

""" % (NVALS,)

s+="""
static float yn_true[] = {
"""

s=complete_array(y,s)

outfile=os.environ['OUTFILE']
with open(outfile,"w") as f:
    f.write(s)
