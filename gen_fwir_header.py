# Generate a header file that configures a struct filter_w_ir_init and a 
# struct limiter_ir_af_init for initializing a struct limiter_ir_af

import os
import numpy as np
import grafted_attenuation_curve as gac

#Do limiting with an IIR generated attenuation curve but whose ramp
#function is arbitrary
try:
    n_p = int(os.environ['N_P'])
except KeyError:
    n_p=300
try:
    n_d = int(os.environ['N_D'])
except KeyError:
    n_d=64000
try:
    buffer_size = int(os.environ['BUFFER_SIZE'])
except KeyError:
    buffer_size = 256
try:
    output_header_file = os.environ['OUTPUT_FILE']
except KeyError:
    output_header_file = "_fwir_test_header.h"
try:
    # in case "const" is not enough to force data to put in the .rodata section
    # (and therefore flash memory on a microcontroller), you can put extra
    # attributes here, e.g., __attribute__((section(".some_section")))
    attrib = os.environ['RODATA_ATTRIBUTES']
except KeyError:
    attrib = ""

print("""
Making attenuation curve with following settings:
n_p={n_p}
n_d={n_d}
buffer_size={buffer_size}
output_header_file={output_header_file}
""".format(n_p=n_p,n_d=n_d,buffer_size=buffer_size,output_header_file=output_header_file))

w,b,a,n_max=gac.atn_fun_design_cos_filter(n_p,n_d)

# atn_fun_design_cos_filter returns attenuation curve of length n_p+n_d but we need one of length n_p + buffer_size, so we truncate or append with 0s as needed
w_trun = np.zeros((n_p+buffer_size,))
_len = np.min([n_p+buffer_size,len(w)])
w_trun[:_len] = w[:_len]

hdr_str = """
#include <stddef.h>
#include "limiter_ir_af.h"
"""

hdr_str += """
const float gen_fwir_header_filter_w_ir_init_a[] %s = {
""" % (attrib,)
for a_ in a[1:]:
    # TODO: Why must this be -a?
    hdr_str += "%.18f, " % (a_,)
hdr_str += "};"

hdr_str += """
const float gen_fwir_header_filter_w_ir_init_ir[] %s = {
""" % (attrib,)
for w_ in w_trun:
    hdr_str += "%.18f, " % (w_,)
hdr_str += "};"

hdr_str += """
const struct filter_w_ir_init gen_fwir_header_filter_w_ir_init %s = {
    .b = NULL,
    .len_b = 0,
    .a = gen_fwir_header_filter_w_ir_init_a,
""" % (attrib,)

# len(a)-1 because atn_fun_design_cos_filter returns y[n] coefficient, which we
# force to be 1
hdr_str += """
    .len_a = %d,""" % (len(a)-1,)
hdr_str += """
    .ir = gen_fwir_header_filter_w_ir_init_ir,"""
hdr_str += """
    .len_ir = %d,
};
""" % (len(w_trun,),)

with open(output_header_file,"w") as f:
    f.write(hdr_str)
