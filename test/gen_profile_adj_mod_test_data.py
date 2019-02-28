# Generate what the adjacent modulo code should output

import numpy as np
import os
from gen_common import complete_array

a=0.99

NVALS=1007

x=np.random.randint(0,1<<32,(NVALS,))
M=np.random.randint(0,1<<32,1)
y0=x%M
y1=(x+1)%M
y2=(x+2)%M

s=""

s+="""
#include <stdint.h>
#define NVALS %d
#define M %d
""" % (NVALS,M)

s+="""
static uint32_t x[] = {
"""

s=complete_array(x,s,"%d,\n")

s+="""
static uint32_t y0[] = {
"""
s=complete_array(y0,s,"%d,\n")

s+="""
static uint32_t y1[] = {
"""
s=complete_array(y1,s,"%d,\n")

s+="""
static uint32_t y2[] = {
"""
s=complete_array(y2,s,"%d,\n")

outfile=os.environ['OUTFILE']
with open(outfile,"w") as f:
    f.write(s)
