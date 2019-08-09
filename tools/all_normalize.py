# Normalize an entire file

from os import environ
import numpy as np
import common

# Load a f32 format file (mono) 
INFILE=environ["INFILE"]
OUTFILE=environ["OUTFILE"]
# W is number of samples over which to determine the normalization factor
W=float(environ["W"])

x=np.fromfile(INFILE,dtype='float32')
y=x-np.mean(x)
y/=np.max(np.abs(x))
y.tofile(OUTFILE)

