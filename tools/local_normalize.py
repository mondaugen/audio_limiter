# Determine a series of factors by which to multiply parts of the audio in order
# to make the loudest samples equal to 1. The factors are interpolated between
# linearly. Because this might cause clipping (imagine a segment of the sound is
# quiet, but the maximum and a value close to it are close in time, a descending
# ramp to the next factor might push the adjacent value above 1, causing
# clipping), you should run the output through a limiter afterwards.

from os import environ
import numpy as np
import common

# Load a f32 format file (mono) 
INFILE=environ["INFILE"]
OUTFILE=environ["OUTFILE"]
# W is number of samples over which to determine the normalization factor
W=float(environ["W"])

x=np.fromfile(INFILE,dtype='float32')
X=common.frame(x,W//2,W)
# determine factors
factors=np.max(np.abs(X),axis=0)
# If maximum was 0, no scaling applied
factors[factors==0]=1
factors=1/factors
# times for the factors, so we can interpolate between them
# we just put the factor at the middle of the window
factor_i=common.hop_indicies(x,W//2,W)+W//2
len_y=(np.max(common.hop_indicies(x,W//2,W))+W).astype('int')
factor_interp=np.interp(np.arange(len_y),factor_i,factors)
y=x[:len_y]*factor_interp
y.astype("float32").tofile(OUTFILE)
