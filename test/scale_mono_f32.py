# Load a f32 format file (mono) and multiply each sample in the file by a
# constant.  No limiting or clipping will be applied to the output file

from os import environ
import numpy as np

INFILE=environ["INFILE"]
OUTFILE=environ["OUTFILE"]
SCALAR=float(environ["SCALAR"])

x=np.fromfile(INFILE,dtype="float32")
y=x*SCALAR

y.tofile(OUTFILE)
