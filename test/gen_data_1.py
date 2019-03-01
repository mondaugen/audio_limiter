# Make some data to test the functions 
import numpy as np
import os
from gen_common import complete_array

outfile=os.environ['OUTFILE']

N_VALS=1000
x1=np.random.standard_normal((N_VALS,))
x2=np.random.standard_normal((N_VALS,))
y1=x1+x2

s="""static float x1_test_data[] = {
"""
s=complete_array(x1,s)

s+="""static float x2_test_data[] = {
"""
s=complete_array(x2,s)

s+="""static float y1_test_data[] = {
"""
s=complete_array(y1,s)

with open(outfile,"w") as f:
    f.write(s)
